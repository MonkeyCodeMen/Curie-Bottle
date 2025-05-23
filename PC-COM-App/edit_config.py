import tkinter as tk
from tkinter import messagebox, simpledialog, ttk
import json
import os

from tkinter import simpledialog

class CustomInputDialog(simpledialog.Dialog):
    def __init__(self, parent, title, prompt):
        self.prompt = prompt
        self.result = None
        super().__init__(parent, title)

    def body(self, master):
        tk.Label(master, text=self.prompt).pack(pady=5)
        self.entry = tk.Entry(master)
        self.entry.pack(pady=5)
        self.entry.focus_set()  # Fokus auf das Eingabefeld setzen
        return self.entry  # Fokus auf das Eingabefeld setzen

    def apply(self):
        self.result = self.entry.get()

def custom_askstring(title, prompt):
    root = tk.Tk()
    root.withdraw()  # Hauptfenster ausblenden
    dialog = CustomInputDialog(root, title, prompt)
    return dialog.result

class EditConfig:
    def __init__(self, app):
        self.app = app
        self.updated_json = None
        self.selected_key = None  # Speichert den aktuell ausgewählten Schlüssel

    def open_edit_config(self, json_data, file_name=""):
        """Create a new window for editing JSON configuration."""
        self.window = tk.Toplevel()
        self.window.title(f"Edit JSON Configuration - {file_name if file_name else 'Unnamed'}")
        self.window.geometry("500x600")

        # JSON-Daten parsen
        try:
            self.config_data = json.loads(json_data)
        except json.JSONDecodeError:
            messagebox.showerror("Error", "Invalid JSON data.")
            self.window.destroy()
            return

        # Buttons für Aktionen über dem Scrollfeld
        button_frame = tk.Frame(self.window)
        button_frame.pack(fill=tk.X, padx=10, pady=5)

        tk.Button(button_frame, text="Add Key", command=self._add_key).pack(side=tk.LEFT, padx=5)
        tk.Button(button_frame, text="Delete Key", command=self._delete_key).pack(side=tk.LEFT, padx=5)
        tk.Button(button_frame, text="Rename Key", command=self._rename_key).pack(side=tk.LEFT, padx=5)
        tk.Button(button_frame, text="Save", command=self._save_config).pack(side=tk.RIGHT, padx=5)

        # Canvas für Scrollbars
        self.canvas = tk.Canvas(self.window)
        self.scrollable_frame = tk.Frame(self.canvas)
        self.scrollbar = tk.Scrollbar(self.window, orient="vertical", command=self.canvas.yview)
        self.canvas.configure(yscrollcommand=self.scrollbar.set)

        self.scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        self.canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        self.canvas.create_window((0, 0), window=self.scrollable_frame, anchor="nw")
        self.scrollable_frame.bind("<Configure>", lambda e: self.canvas.configure(scrollregion=self.canvas.bbox("all")))

        # Konfiguration anzeigen
        self._display_config()

    def _display_config(self):
        """Display the configuration in a two-column layout."""
        for widget in self.scrollable_frame.winfo_children():
            widget.destroy()

        tk.Label(self.scrollable_frame, text="Key", font=("Arial", 10, "bold")).grid(row=0, column=0, padx=5, pady=5, sticky="w")
        tk.Label(self.scrollable_frame, text="Value", font=("Arial", 10, "bold")).grid(row=0, column=1, padx=5, pady=5, sticky="w")

        self.entries = {}
        for i, (key, value) in enumerate(self.config_data.items(), start=1):
            key_label = tk.Label(self.scrollable_frame, text=key)
            key_label.grid(row=i, column=0, padx=5, pady=5, sticky="w")
            key_label.bind("<Button-1>", lambda e, k=key: self._select_key(k))  # Schlüssel auswählen

            entry = tk.Entry(self.scrollable_frame, width=40)
            entry.insert(0, value)
            entry.grid(row=i, column=1, padx=5, pady=5, sticky="w")
            self.entries[key] = entry

    def _select_key(self, key):
        """Set the selected key."""
        self.selected_key = key

    def _get_selected_key(self):
        """Provide dropdown menu to select a key."""
        key_selection_window = tk.Toplevel(self.window)
        key_selection_window.title("Select Key")
        key_selection_window.geometry("300x100")

        tk.Label(key_selection_window, text="Select a key:").pack(pady=5)
        key_var = tk.StringVar(value=list(self.config_data.keys())[0])
        key_dropdown = ttk.Combobox(key_selection_window, textvariable=key_var, values=list(self.config_data.keys()), state="readonly")
        key_dropdown.pack(pady=5)

        def confirm_selection():
            self.selected_key = key_var.get()
            key_selection_window.destroy()

        tk.Button(key_selection_window, text="OK", command=confirm_selection).pack(pady=5)
        key_selection_window.wait_window()

        return self.selected_key

    def _add_key(self):
        """Add a new key to the JSON configuration."""
        new_key = custom_askstring("Add Key", "Enter the new key:")
        if not new_key:
            return

        if new_key in self.config_data:
            messagebox.showerror("Error", "Key already exists.")
            return

        new_value = custom_askstring("Add Key", "Enter the value for the new key:")
        self.config_data[new_key] = new_value if new_value else ""
        self._display_config()

    def _delete_key(self):
        """Delete a key from the JSON configuration."""
        key_to_delete = self._get_selected_key()
        if not key_to_delete:
            return

        if key_to_delete not in self.config_data:
            messagebox.showerror("Error", "Key not found.")
            return

        del self.config_data[key_to_delete]
        self.selected_key = None  # Auswahl zurücksetzen
        self._display_config()

    def _rename_key(self):
        """Rename a key in the JSON configuration."""
        old_key = self._get_selected_key()
        if not old_key:
            return

        if old_key not in self.config_data:
            messagebox.showerror("Error", "Key not found.")
            return

        new_key = simpledialog.askstring("Rename Key", "Enter the new key name:")
        if not new_key:
            return

        if new_key in self.config_data:
            messagebox.showerror("Error", "Key already exists.")
            return

        self.config_data[new_key] = self.config_data.pop(old_key)
        self.selected_key = new_key  # Auswahl auf den neuen Schlüssel setzen
        self._display_config()

    def _save_config(self):
        """Save the updated configuration."""
        # Aktualisiere die Konfigurationsdaten mit den Werten aus den Eingabefeldern
        for key, entry in self.entries.items():
            self.config_data[key] = entry.get()

        # Speichere die aktualisierte Konfiguration als JSON-String
        self.updated_json = json.dumps(self.config_data, indent=4)
        messagebox.showinfo("Success", "Configuration updated successfully!")
        self.window.destroy()

    def get_updated_json(self):
        """Return the updated JSON string."""
        return self.updated_json