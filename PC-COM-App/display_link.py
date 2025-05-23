import tkinter as tk
from tkinter import filedialog, messagebox, ttk
from serial_comm import SerialCommunicator
from ProgressWindow import ProgressWindow

class DisplayLink:
    def __init__(self, main_app, work_frame, serial_comm):
        self.frame = work_frame
        self.serial_comm = serial_comm
        self.app = main_app

        # Erstelle zwei Frames: Links für Befehle und Aktionen, rechts für die Display-Ausgabe
        self.cmd_frame = tk.Frame(self.frame)
        self.cmd_frame.pack(side=tk.LEFT, padx=5, pady=5)
        self.data_frame = tk.Frame(self.frame)
        self.data_frame.pack(side=tk.LEFT, expand=True, fill=tk.BOTH, padx=5, pady=5)

        # Auswahl des Displays
        tk.Label(self.cmd_frame, text="Select Display page:").pack(side=tk.TOP, pady=5)
        def_page = "255: (current page)"
        self.display_var = tk.StringVar(value=def_page)
        display_options = [def_page] + [f"{i}: page " for i in range(0, 19)]
        self.display_menu = ttk.Combobox(self.cmd_frame, textvariable=self.display_var, values=display_options, state="readonly")
        self.display_menu.pack(side=tk.TOP, pady=20)

        # Auswahl des Abfragemodus
        tk.Label(self.cmd_frame, text="Query Mode:").pack(side=tk.TOP, pady=5)
        self.query_mode = tk.StringVar(value="Once")
        tk.Radiobutton(self.cmd_frame, text="Once", variable=self.query_mode, value="Once").pack(side=tk.TOP, anchor="w", padx=10)
        tk.Radiobutton(self.cmd_frame, text="Cyclic", variable=self.query_mode, value="Cyclic").pack(side=tk.TOP, anchor="w", padx=10)

        # Button zum Starten der Abfrage
        tk.Button(self.cmd_frame, text="Fetch Display", command=self.fetch_display).pack(side=tk.TOP, pady=20)

        # Bereich zur Anzeige der Display-Daten
        self.display_output = tk.Text(self.data_frame, state=tk.DISABLED)
        self.display_output.pack(side=tk.TOP, fill=tk.BOTH, expand=True, pady=5)

        # Schließen-Button
        tk.Button(self.cmd_frame, text="Close", command=self._on_close).pack(side=tk.BOTTOM, pady=50)

    def fetch_display(self):
        """Fordert die Display-Daten vom RP2040 an."""
        page_entry = self.display_var.get()
        query_mode = self.query_mode.get()

        # Erstelle den Befehl im gewünschten Format
        page = page_entry.split(":")[0]  # Extrahiere die Seiten-ID
        command = f'S:D0,read,{page}#'

        # Sende den Befehl und empfange die Antwort
        response = self.serial_comm.send(command)

        response = response.split("OK-Display Info:")[1] if "OK-Display Info:" in response else "NOK answer\n"
        info = response.split("Content:")[0] if "Content:" in response else "invalid answer\n"
        content = response.split("Content:")[1] if "Content:" in response else "invalid answer\n"
        content = content[:-2] + "\n"
        msg = f"Display page requested: {page}\n" + "########### Info ############" + info + "########### Content ############" + content + "###############################\n"

        # Zeige die Antwort im Textfeld an
        self.display_output.config(state=tk.NORMAL)
        self.display_output.delete("1.0", tk.END)
        self.display_output.insert(tk.END, msg)
        self.display_output.config(state=tk.DISABLED)

        # Falls zyklisch, starte die wiederholte Abfrage
        if query_mode == "Cyclic":
            self.frame.after(1000, self.fetch_display)  # Wiederhole alle 1000 ms

    def _on_close(self):
        # Schließe den Display-Dialog
        self.cyclic_update = False
        # Zeige das Hauptmenü wieder an
        self.app._show_selection_menu()