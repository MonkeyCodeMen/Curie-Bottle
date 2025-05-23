# -*- coding: utf-8 -*-
"""
Created on Fri Nov 29 21:56:58 2024

@author: MonekyCodeMen
"""

import os
current_directory = os.getcwd()
print("Aktuelles Arbeitsverzeichnis:", current_directory)
if (not current_directory.endswith('PC-COM-App')):
    os.chdir('PC-COM-App')
    current_directory = os.getcwd()
    print("Aktuelles Arbeitsverzeichnis:", current_directory)

    
import tkinter as tk
from tkinter import ttk
from serial_comm import SerialCommunicator
from mode_handlers import ModeHandler
from file_manager import FileManager    # modul for file manager of LittleFS
from edit_config import EditConfig      # Sub-App for file manager
from display_link import DisplayLink    # modul for display link
from dump_link import DumpLink
import time
import serial
import serial.tools.list_ports


class PicoCommandApp:
    def __init__(self, master):
        self.master = master
        self.master.title("Pico Serial Commander")
        self.master.geometry("1400x800")

        self.mode_handler = ModeHandler(self)
        self.serial_comm = SerialCommunicator(self)  # Modul zur Handhabung der seriellen Kommunikation
        self.file_manager = FileManager(self)

        
        # Hauptfenster
        self.main_frame = tk.Frame(self.master)
        self.main_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        # Verbindungsbereich
        self.connection_frame = tk.LabelFrame(self.main_frame, text="Connection")
        self.connection_frame.pack(side=tk.TOP, fill=tk.X, pady=5)
        self._setup_connection_area(self.connection_frame)

        # Arbeitsbereich und Buttons
        self.work_frame = tk.LabelFrame(self.main_frame, text="No function selected")
        self.work_frame.pack(side=tk.TOP, fill=tk.BOTH, expand=True, padx=5, pady=5)

        # Nachrichtenbereich
        self.message_frame = tk.LabelFrame(self.main_frame, text="Frames Log")
        self.message_frame.pack(side=tk.BOTTOM, fill=tk.BOTH, expand=True, pady=5)
        self._setup_message_area(self.message_frame)

        master.protocol("WM_DELETE_WINDOW", self._on_close)

    def __del__(self):
        self.serial_comm.disconnect()

    def _on_close(self):
        self.master.quit()
        self.master.destroy()

    def _clear_frame(self, frame):
        """Clears the content of the frame before adding new content."""
        for widget in frame.winfo_children():
            widget.destroy()

    def _setup_connection_area(self, frame):
        # Auswahl des Ports
        tk.Label(frame, text="Port:").pack(side=tk.LEFT, padx=5)
        self.port_var = tk.StringVar()
        ports = [port.device for port in serial.tools.list_ports.comports()]
        self.port_menu = ttk.Combobox(frame, textvariable=self.port_var, values=ports, state="readonly")
        self.port_menu.pack(side=tk.LEFT)

        # Auswahl der Baudrate
        tk.Label(frame, text="Baudrate:").pack(side=tk.LEFT, padx=5)
        self.baud_var = tk.StringVar(value="115200")
        self.baud_menu = ttk.Combobox(
            frame, textvariable=self.baud_var, values=["9600", "115200", "19200", "38400", "57600"], state="readonly"
        )
        self.baud_menu.pack(side=tk.LEFT)

        # Verbinden/Trennen-Button
        self.connect_button = tk.Button(frame, text="Connect", command=self.toggle_connection)
        self.connect_button.pack(side=tk.LEFT, padx=5, pady=5)

        # Verbindungsstatus
        self.connection_status_label = tk.Label(frame, text="Disconnected", width=20)
        self.connection_status_label.pack(side=tk.LEFT, padx=5)

    def _setup_message_area(self, frame):
        # Nachrichtenprotokoll
        self.frame_log = tk.Text(frame, state=tk.DISABLED, bg="lightgrey", height=15)
        self.frame_log.pack(side=tk.TOP, fill=tk.BOTH, expand=True, padx=5, pady=5)
        scroll = tk.Scrollbar(self.frame_log, command=self.frame_log.yview)
        scroll.pack(side=tk.RIGHT, fill=tk.Y)
        self.frame_log.configure(yscrollcommand=scroll.set)
        tk.Button(frame, text="Clear Log", command=self._clear_log).pack(side=tk.LEFT, pady=2, padx=10)
        tk.Button(frame, text="Save Log", command=self._save_log_to_file).pack(side=tk.LEFT, pady=2, padx=10)
        self.log_buffer=""


      



    def toggle_connection(self):
        """Connect or disconnect based on the current state."""
        if self.serial_comm.is_connected():
            self._clear_frame(self.work_frame)
            self.work_frame.config(text="No function selected")
            self.serial_comm.disconnect()
            self.connection_status_label.config(text="Disconnected")
            self.connect_button.config(text="Connect")
        else:
            self.serial_comm.connect(self.port_var.get(), int(self.baud_var.get()))
            self.connection_status_label.config(text="Connected")
            self.connect_button.config(text="Disconnect")
            self._show_selection_menu()

    def _show_selection_menu(self):
        """Zeigt eine Auswahloberfläche für verschiedene Dialoge."""
        self._clear_frame(self.work_frame)
        self.work_frame.config(text="Select Function")

        tk.Button(self.work_frame, text="File Manager",     command=self._open_file_manager     ).pack(side=tk.LEFT, padx=5, pady=5)
        tk.Button(self.work_frame, text="Display Dialog",   command=self.open_display_dialog    ).pack(side=tk.LEFT, padx=5, pady=5)
        tk.Button(self.work_frame, text="Dump",             command=self._open_dump_dialog      ).pack(side=tk.LEFT, padx=5, pady=5)

    def _open_dump_dialog(self):
        """Zeigt das Dump-Modul an."""
        self._clear_frame(self.work_frame)
        self.work_frame.config(text="Dump module")
        DumpLink(self, self.work_frame, self.serial_comm)

    def _open_file_manager(self):
        """Zeigt den File-Manager im work_frame an."""
        self._clear_frame(self.work_frame)  # Entfernt den aktuellen Inhalt des work_frame
        self.work_frame.config(text="File Manager")
        # File-Manager anzeigen
        self.file_manager.open_file_manager(self, self.work_frame, self.serial_comm)
        # button close of the display dialog will call the function _show_selection_menu to show the main menu again


    def open_display_dialog(self):
        """Zeigt den Display-Dialog im work_frame an."""
        self._clear_frame(self.work_frame)  # Entfernt den aktuellen Inhalt des work_frame
        self.work_frame.config(text="Display module")
        # Display-Dialog anzeigen
        DisplayLink(self, self.work_frame, self.serial_comm)
        # button close of the display dialog will call the function _show_selection_menu to show the main menu again


    def _log_recv(self, data):
        self._check_log_buffer()
        timestamp = time.strftime("%H:%M:%S", time.localtime())
        self.frame_log.config(state='normal')
        self.frame_log.insert(tk.END, f"[RECV] {timestamp}: {data}\n")
        self.frame_log.see(tk.END)
        self.frame_log.config(state='disabled')

    def _log_sent(self, data):
        self._check_log_buffer()
        timestamp = time.strftime("%H:%M:%S", time.localtime())
        self.frame_log.config(state='normal')
        self.frame_log.insert(tk.END, f"[SENT] {timestamp}: {data}\n")
        self.frame_log.see(tk.END)
        self.frame_log.config(state='disabled')
    
    def _check_log_buffer(self):
        if self.log_buffer != "":
            self.frame_log.config(state='normal')
            self.frame_log.insert(tk.END, self.log_buffer)
            self.frame_log.see(tk.END)
            self.frame_log.config(state='disabled')
            self.log_buffer = ""
        

    def _log_recv_buffered(self, data):
        timestamp = time.strftime("%H:%M:%S", time.localtime())
        self.log_buffer+=f"[RECV] {timestamp}: {data}\n"

    def _log_sent_buffered(self, data):
        timestamp = time.strftime("%H:%M:%S", time.localtime())
        self.log_buffer+=f"[SENT] {timestamp}: {data}\n"


    def _clear_log(self):
        self.log_buffer = ""
        self.frame_log.config(state='normal')
        self.frame_log.delete("1.0", tk.END)  # Clear the send message window
        self.frame_log.config(state='disabled')
        
    def _save_log_to_file(self):
        """write current log to file """
        log_content = self.frame_log.get("1.0", tk.END)  # Holt den gesamten Inhalt des Text-Widgets
        
        # Öffnen der Datei im Anhängemodus
        with open("frame_log.txt", "a") as log_file:
            log_file.write(log_content)  # Schreibe den Inhalt in die Datei



if __name__ == "__main__":
    root = tk.Tk()
    app = PicoCommandApp(root)
    root.mainloop()



