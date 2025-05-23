import tkinter as tk
from tkinter import ttk


class DumpLink:
    def __init__(self, main_app, work_frame, serial_comm):
        self.app = main_app
        self.frame = work_frame
        self.serial_comm = serial_comm

        # Frames für die GUI
        self.cmd_frame = tk.Frame(self.frame)
        self.cmd_frame.pack(side=tk.LEFT, padx=5, pady=5)
        self.data_frame = tk.Frame(self.frame)
        self.data_frame.pack(side=tk.LEFT, expand=True, fill=tk.BOTH, padx=5, pady=5)

        # Auswahl der verfügbaren Module
        tk.Label(self.cmd_frame, text="Available Dump Modules:").pack(side=tk.TOP, pady=5)
        self.module_var = tk.StringVar(value="Select a module")
        self.module_listbox = tk.Listbox(self.cmd_frame, listvariable=self.module_var, height=10)
        self.module_listbox.pack(side=tk.TOP, pady=5)

        # Auswahl des Abfragemodus
        tk.Label(self.cmd_frame, text="Query Mode:").pack(side=tk.TOP, pady=5)
        self.query_mode = tk.StringVar(value="Once")
        tk.Radiobutton(self.cmd_frame, text="Once", variable=self.query_mode, value="Once").pack(side=tk.TOP, anchor="w", padx=10)
        tk.Radiobutton(self.cmd_frame, text="Cyclic", variable=self.query_mode, value="Cyclic").pack(side=tk.TOP, anchor="w", padx=10)

        # Button zum Starten der Abfrage
        tk.Button(self.cmd_frame, text="Fetch Dump", command=self.fetch_dump).pack(side=tk.TOP, pady=20)

        # Bereich zur Anzeige der Dump-Daten
        self.dump_output = tk.Text(self.data_frame, state=tk.DISABLED, bg="lightgrey", height=15)
        self.dump_output.pack(side=tk.TOP, fill=tk.BOTH, expand=True, padx=5, pady=5)

        # Schließen-Button
        tk.Button(self.cmd_frame, text="Close", command=self._on_close).pack(side=tk.BOTTOM, pady=50)

        # Lade die verfügbaren Module
        self.load_modules()

    def load_modules(self):
        """Lädt die verfügbaren Dump-Module über das `list`-Kommando."""
        response = self.serial_comm.send("S:I0,list#")
        if "OK-Dumper list:" in response:
            answer = response.split("OK-Dumper list:")[1]

            if "#" in answer:

                answer = answer[:answer.rfind("#")] + answer[answer.rfind("#") + 1:]
                modules = answer.split(", ")
                self.module_listbox.delete(0, tk.END)
                for module in modules:
                    self.module_listbox.insert(tk.END, module)
                return
        self.app._log_recv_buffered(f"Error loading modules: {response}")

    def fetch_dump(self):
        """Fordert die Dump-Daten für das ausgewählte Modul an."""
        selected_module = self.module_listbox.get(tk.ACTIVE)
        if not selected_module:
            self.app._log_recv_buffered("No module selected.")
            return

        command = f'S:I0,dump,0,0,0,0,"{selected_module}"#'
        response = self.serial_comm.send(command)
        if ("OK-Dumper dump:") in response:
            answer = response.split("OK-Dumper dump:")[1]
            if "#" in answer:
                answer = answer[:answer.rfind("#")] + answer[answer.rfind("#") + 1:]
                self.display_dump(answer)
                # Falls zyklisch, starte die wiederholte Abfrage
                if self.query_mode.get() == "Cyclic":
                    self.frame.after(1000, self.fetch_dump)

    def display_dump(self, dump_data):
        """Zeigt die Dump-Daten im Textfeld an."""
        self.dump_output.config(state=tk.NORMAL)
        self.dump_output.delete("1.0", tk.END)
        self.dump_output.insert(tk.END, dump_data)
        self.dump_output.config(state=tk.DISABLED)

    def _on_close(self):
        """Schließt das Modul und kehrt zur Hauptauswahl zurück."""
        # Zeige das Hauptmenü wieder an
        self.app._show_selection_menu()