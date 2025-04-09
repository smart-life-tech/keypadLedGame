import tkinter as tk
from tkinter import ttk, messagebox, filedialog
import serial
import serial.tools.list_ports
import time
import json
import os

class KeypadGameConfigurator:
    def __init__(self, root):
        self.root = root
        self.root.title("Keypad LED Game Configurator")
        self.root.geometry("800x600")
        
        self.serial_port = None
        
        # Create variables for storing configuration
        self.countdown_var = tk.StringVar()
        self.penalty_var = tk.StringVar()
        self.switch_vars = []
        self.button_vars = []
        self.pot_vars = []
        self.jack_vars = []
        self.keypad_var = tk.StringVar()
        self.mp3_start_var = tk.StringVar()
        self.mp3_victory_var = tk.StringVar()
        self.mp3_defeat_var = tk.StringVar()
        self.mp3_success_var = tk.StringVar()
        self.mp3_failure_var = tk.StringVar()
        
        # Create main frame
        main_frame = ttk.Frame(root, padding="10")
        main_frame.pack(fill=tk.BOTH, expand=True)
        
        # Create connection frame
        connection_frame = ttk.LabelFrame(main_frame, text="Arduino Connection", padding="10")
        connection_frame.pack(fill=tk.X, pady=5)
        
        # COM port selection
        ttk.Label(connection_frame, text="COM Port:").grid(row=0, column=0, sticky=tk.W, padx=5, pady=5)
        self.port_combobox = ttk.Combobox(connection_frame, width=20)
        self.port_combobox.grid(row=0, column=1, padx=5, pady=5)
        
        # Refresh and connect buttons
        ttk.Button(connection_frame, text="Refresh Ports", command=self.refresh_ports).grid(row=0, column=2, padx=5, pady=5)
        self.connect_button = ttk.Button(connection_frame, text="Connect", command=self.connect_to_arduino)
        self.connect_button.grid(row=0, column=3, padx=5, pady=5)
        
        # Create notebook for different configuration sections
        self.notebook = ttk.Notebook(main_frame)
        self.notebook.pack(fill=tk.BOTH, expand=True, pady=10)
        
        # Create tabs
        self.create_general_tab()
        self.create_switches_tab()
        self.create_buttons_tab()
        self.create_pots_tab()
        self.create_jacks_tab()
        self.create_keypad_tab()
        
        # Create bottom frame for save/load/send buttons
        bottom_frame = ttk.Frame(main_frame)
        bottom_frame.pack(fill=tk.X, pady=10)
        
        ttk.Button(bottom_frame, text="Load Configuration", command=self.load_config).pack(side=tk.LEFT, padx=5)
        ttk.Button(bottom_frame, text="Save Configuration", command=self.save_config).pack(side=tk.LEFT, padx=5)
        ttk.Button(bottom_frame, text="Send to Arduino", command=self.send_to_arduino).pack(side=tk.LEFT, padx=5)
        ttk.Button(bottom_frame, text="Get from Arduino", command=self.get_from_arduino).pack(side=tk.LEFT, padx=5)
        ttk.Button(bottom_frame, text="Reset Game", command=self.reset_game).pack(side=tk.RIGHT, padx=5)
        
        # Initialize with default values
        self.refresh_ports()
        self.initialize_default_values()
    
    def create_general_tab(self):
        general_frame = ttk.Frame(self.notebook, padding="10")
        self.notebook.add(general_frame, text="General Settings")
        
        # Countdown timer
        ttk.Label(general_frame, text="Countdown Time (seconds):").grid(row=0, column=0, sticky=tk.W, padx=5, pady=5)
        ttk.Entry(general_frame, textvariable=self.countdown_var, width=10).grid(row=0, column=1, sticky=tk.W, padx=5, pady=5)
        
        # Penalty time
        ttk.Label(general_frame, text="Penalty Time (seconds):").grid(row=1, column=0, sticky=tk.W, padx=5, pady=5)
        ttk.Entry(general_frame, textvariable=self.penalty_var, width=10).grid(row=1, column=1, sticky=tk.W, padx=5, pady=5)
        
        # MP3 file settings
        ttk.Label(general_frame, text="MP3 Files:").grid(row=2, column=0, sticky=tk.W, padx=5, pady=5)
        
        mp3_frame = ttk.Frame(general_frame)
        mp3_frame.grid(row=3, column=0, columnspan=2, sticky=tk.W, padx=5, pady=5)
        
        ttk.Label(mp3_frame, text="1. Start Game:").grid(row=0, column=0, sticky=tk.W, padx=5, pady=2)
        ttk.Entry(mp3_frame, textvariable=self.mp3_start_var, width=30).grid(row=0, column=1, sticky=tk.W, padx=5, pady=2)
        ttk.Button(mp3_frame, text="Browse", command=lambda: self.browse_mp3(self.mp3_start_var)).grid(row=0, column=2, padx=5, pady=2)
        
        ttk.Label(mp3_frame, text="2. Victory:").grid(row=1, column=0, sticky=tk.W, padx=5, pady=2)
        ttk.Entry(mp3_frame, textvariable=self.mp3_victory_var, width=30).grid(row=1, column=1, sticky=tk.W, padx=5, pady=2)
        ttk.Button(mp3_frame, text="Browse", command=lambda: self.browse_mp3(self.mp3_victory_var)).grid(row=1, column=2, padx=5, pady=2)
        
        ttk.Label(mp3_frame, text="3. Defeat:").grid(row=2, column=0, sticky=tk.W, padx=5, pady=2)
        ttk.Entry(mp3_frame, textvariable=self.mp3_defeat_var, width=30).grid(row=2, column=1, sticky=tk.W, padx=5, pady=2)
        ttk.Button(mp3_frame, text="Browse", command=lambda: self.browse_mp3(self.mp3_defeat_var)).grid(row=2, column=2, padx=5, pady=2)
        
        ttk.Label(mp3_frame, text="4. Success:").grid(row=3, column=0, sticky=tk.W, padx=5, pady=2)
        ttk.Entry(mp3_frame, textvariable=self.mp3_success_var, width=30).grid(row=3, column=1, sticky=tk.W, padx=5, pady=2)
        ttk.Button(mp3_frame, text="Browse", command=lambda: self.browse_mp3(self.mp3_success_var)).grid(row=3, column=2, padx=5, pady=2)
        
        ttk.Label(mp3_frame, text="5. Failure:").grid(row=4, column=0, sticky=tk.W, padx=5, pady=2)
        ttk.Entry(mp3_frame, textvariable=self.mp3_failure_var, width=30).grid(row=4, column=1, sticky=tk.W, padx=5, pady=2)
        ttk.Button(mp3_frame, text="Browse", command=lambda: self.browse_mp3(self.mp3_failure_var)).grid(row=4, column=2, padx=5, pady=2)
        
        # Upload MP3 files button
        ttk.Button(general_frame, text="Upload MP3 Files to Arduino", command=self.upload_mp3_files).grid(row=4, column=0, columnspan=2, pady=10)
    def load_config(self):
        filename = filedialog.askopenfilename(
            title="Load Configuration",
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
        )
        if not filename:
            return
        
        try:
            with open(filename, 'r') as f:
                config = json.load(f)
            
            # Load values from config
            self.countdown_var.set(str(config.get('countdown', 300)))
            self.penalty_var.set(str(config.get('penalty', 30)))
            
            # Load switch positions
            switch_positions = config.get('switches', ['0'] * 6)
            for i, pos in enumerate(switch_positions):
                if i < len(self.switch_vars):
                    self.switch_vars[i].set(pos)
            
            # Load button sequence
            button_sequence = config.get('buttons', [str(i+1) for i in range(6)])
            for i, btn in enumerate(button_sequence):
                if i < len(self.button_vars):
                    self.button_vars[i].set(btn)
            
            # Load potentiometer values
            pot_values = config.get('pots', ['512'] * 6)
            for i, val in enumerate(pot_values):
                if i < len(self.pot_vars):
                    self.pot_vars[i].set(val)
            
            # Load jack connections
            jack_connections = config.get('jacks', [['0', '1']] * 8)
            for i, (jack1, jack2) in enumerate(jack_connections):
                if i < len(self.jack_vars):
                    self.jack_vars[i][0].set(jack1)
                    self.jack_vars[i][1].set(jack2)
            
            # Load keypad code
            self.keypad_var.set(config.get('keypad_code', '1234'))
            
            # Load MP3 file paths
            self.mp3_start_var.set(config.get('mp3_start', ''))
            self.mp3_victory_var.set(config.get('mp3_victory', ''))
            self.mp3_defeat_var.set(config.get('mp3_defeat', ''))
            self.mp3_success_var.set(config.get('mp3_success', ''))
            self.mp3_failure_var.set(config.get('mp3_failure', ''))
            
            messagebox.showinfo("Load Configuration", "Configuration loaded successfully")
        except Exception as e:
            messagebox.showerror("Load Error", f"Failed to load configuration: {str(e)}")
    
    def save_config(self):
        filename = filedialog.asksaveasfilename(
            title="Save Configuration",
            defaultextension=".json",
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
        )
        if not filename:
            return
        
        try:
            config = {
                'countdown': int(self.countdown_var.get()),
                'penalty': int(self.penalty_var.get()),
                'switches': [var.get() for var in self.switch_vars],
                'buttons': [var.get() for var in self.button_vars],
                'pots': [var.get() for var in self.pot_vars],
                'jacks': [[jack1_var.get(), jack2_var.get()] for jack1_var, jack2_var in self.jack_vars],
                'keypad_code': self.keypad_var.get(),
                'mp3_start': self.mp3_start_var.get(),
                'mp3_victory': self.mp3_victory_var.get(),
                'mp3_defeat': self.mp3_defeat_var.get(),
                'mp3_success': self.mp3_success_var.get(),
                'mp3_failure': self.mp3_failure_var.get()
            }
            
            with open(filename, 'w') as f:
                json.dump(config, f, indent=4)
            
            messagebox.showinfo("Save Configuration", "Configuration saved successfully")
        except Exception as e:
            messagebox.showerror("Save Error", f"Failed to save configuration: {str(e)}")
    
    def refresh_ports(self):
        """Refresh the list of available COM ports"""
        ports = [port.device for port in serial.tools.list_ports.comports()]
        self.port_combobox['values'] = ports
        if ports:
            self.port_combobox.current(0)

    def connect_to_arduino(self):
        """Establish a connection to the Arduino"""
        port = self.port_combobox.get()
        try:
            self.serial_port = serial.Serial(port, 9600, timeout=1)
            time.sleep(2)  # Wait for Arduino to reset
            messagebox.showinfo("Connection", f"Connected to Arduino on {port}")
            self.connect_button.config(text="Disconnect", command=self.disconnect_from_arduino)
        except Exception as e:
            messagebox.showerror("Connection Error", f"Failed to connect: {str(e)}")

    def disconnect_from_arduino(self):
        """Disconnect from the Arduino"""
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.close()
        self.serial_port = None
        self.connect_button.config(text="Connect", command=self.connect_to_arduino)
        messagebox.showinfo("Disconnection", "Disconnected from Arduino")

    def send_command(self, command):
        """Send a command to the Arduino and wait for response"""
        if not self.serial_port or not self.serial_port.is_open:
            raise Exception("Not connected to Arduino")
        print("sending :",command)
        self.serial_port.write(f"{command}\n".encode('utf-8'))
        time.sleep(1)  # Give Arduino time to process
        
        raw_response = self.serial_port.readline()
        print(f"Raw response bytes: {raw_response}")
        
        try:
            response = raw_response.decode('utf-8').strip()
            print(f"Decoded response: {response}")
        except UnicodeDecodeError as e:
            print(f"Decoding error: {e}")
            raise Exception(f"Decoding error: {e}")
        
        if not response.startswith("OK:"):
            raise Exception(f"Unexpected response: {response}")
        
        return response

    def upload_mp3_files(self):
        """Upload MP3 files to the Arduino's SD card"""
        # This would typically require a separate SD card reader/writer
        # For now, just show instructions for manual copying
        messagebox.showinfo("MP3 Upload", 
                       "Please manually copy the MP3 files to the SD card with the following names:\n\n"
                       "1.mp3 - Start Game\n"
                       "2.mp3 - Victory\n"
                       "3.mp3 - Defeat\n"
                       "4.mp3 - Success\n"
                       "5.mp3 - Failure")

    def browse_mp3(self, var):
        """Browse for an MP3 file and update the variable"""
        filename = filedialog.askopenfilename(
            title="Select MP3 File",
            filetypes=[("MP3 files", "*.mp3"), ("All files", "*.*")]
        )
        if filename:
            var.set(filename)

    def send_to_arduino(self):
        """Send the current configuration to the Arduino"""
        if not self.serial_port or not self.serial_port.is_open:
            messagebox.showerror("Connection Error", "Not connected to Arduino")
            return
        
        try:
            # Send countdown time
            self.send_command(f"SET:COUNTDOWN:{self.countdown_var.get()}")
            
            # Send penalty time
            self.send_command(f"SET:PENALTY:{self.penalty_var.get()}")
            
            # Send switch positions
            switch_str = ''.join([var.get() for var in self.switch_vars])
            self.send_command(f"SET:SWITCHES:{switch_str}")
            
            # Send button sequence
            button_str = ''.join([var.get() for var in self.button_vars])
            self.send_command(f"SET:BUTTONS:{button_str}")
            
            # Send potentiometer values
            pot_str = ','.join([var.get() for var in self.pot_vars])
            self.send_command(f"SET:POTS:{pot_str}")
            
            # Send jack connections
            jack_str = ','.join([f"{jack1_var.get()},{jack2_var.get()}" for jack1_var, jack2_var in self.jack_vars])
            self.send_command(f"SET:JACKS:{jack_str}")
            
            # Send keypad code
            self.send_command(f"SET:KEYCODE:{self.keypad_var.get()}")
            
            # Save configuration to EEPROM
            self.send_command("SET:SAVE")
            
            messagebox.showinfo("Configuration", "Configuration sent to Arduino successfully")
        except Exception as e:
            messagebox.showerror("Communication Error", f"Failed to send configuration: {str(e)}")

    def get_from_arduino(self):
        """Get the current configuration from the Arduino"""
        if not self.serial_port or not self.serial_port.is_open:
            messagebox.showerror("Connection Error", "Not connected to Arduino")
            return
        
        try:
            # Request current configuration
            self.serial_port.write(b"GET:CONFIG\n")
            
            # Read responses
            config_data = {}
            for _ in range(7):  # Expect 7 configuration lines
                response = self.serial_port.readline().decode('utf-8').strip()
                if response.startswith("CONFIG:"):
                    parts = response[7:].split(":", 1)
                    if len(parts) == 2:
                        config_data[parts[0]] = parts[1]
            
            # Update UI with received values
            if 'COUNTDOWN' in config_data:
                self.countdown_var.set(config_data['COUNTDOWN'])
            
            if 'PENALTY' in config_data:
                self.penalty_var.set(config_data['PENALTY'])
            
            if 'SWITCHES' in config_data:
                for i, val in enumerate(config_data['SWITCHES']):
                    if i < len(self.switch_vars):
                        self.switch_vars[i].set(val)
            
            if 'BUTTONS' in config_data:
                for i, val in enumerate(config_data['BUTTONS']):
                    if i < len(self.button_vars):
                        self.button_vars[i].set(val)
            
            if 'POTS' in config_data:
                pot_values = config_data['POTS'].split(',')
                for i, val in enumerate(pot_values):
                    if i < len(self.pot_vars):
                        self.pot_vars[i].set(val)
            
            if 'JACKS' in config_data:
                jack_values = config_data['JACKS'].split(',')
                for i in range(0, len(jack_values), 2):
                    idx = i // 2
                    if idx < len(self.jack_vars) and i+1 < len(jack_values):
                        self.jack_vars[idx][0].set(jack_values[i])
                        self.jack_vars[idx][1].set(jack_values[i+1])
            
            if 'KEYCODE' in config_data:
                self.keypad_var.set(config_data['KEYCODE'])
            
            messagebox.showinfo("Configuration", "Configuration retrieved from Arduino successfully")
        except Exception as e:
            messagebox.showerror("Communication Error", f"Failed to get configuration: {str(e)}")

    def reset_game(self):
        """Reset the game on the Arduino"""
        if not self.serial_port or not self.serial_port.is_open:
            messagebox.showerror("Connection Error", "Not connected to Arduino")
            return
        
        try:
            self.send_command("RESET")
            messagebox.showinfo("Reset", "Game reset successfully")
        except Exception as e:
            messagebox.showerror("Communication Error", f"Failed to reset game: {str(e)}")
    
    def initialize_default_values(self):
        # Set default values
        self.countdown_var.set("300")  # 5 minutes
        self.penalty_var.set("30")     # 30 seconds penalty
        
        # Default switch positions (all OFF)
        for var in self.switch_vars:
            var.set("0")
        
        # Default button sequence (1,2,3,4,5,6)
        for i, var in enumerate(self.button_vars):
            var.set(str(i+1))
        
        # Default potentiometer values (mid-range)
        for var in self.pot_vars:
            var.set("512")
        
        # Default jack connections (none)
        for jack1_var, jack2_var in self.jack_vars:
            jack1_var.set("0")
            jack2_var.set("1")
        
        # Default keypad code
        self.keypad_var.set("1234")
        
        # Default MP3 files
        self.mp3_start_var.set("")
        self.mp3_victory_var.set("")
        self.mp3_defeat_var.set("")
        self.mp3_success_var.set("")
        self.mp3_failure_var.set("")

    def create_switches_tab(self):
        switches_frame = ttk.Frame(self.notebook, padding="10")
        self.notebook.add(switches_frame, text="Switches")
        
        ttk.Label(switches_frame, text="Set the correct switch positions (ON=1, OFF=0):").grid(row=0, column=0, columnspan=6, sticky=tk.W, padx=5, pady=5)
        
        self.switch_vars = []
        for i in range(6):
            var = tk.StringVar()
            self.switch_vars.append(var)
            ttk.Label(switches_frame, text=f"Switch {i+1}:").grid(row=1, column=i, padx=5, pady=5)
            ttk.Combobox(switches_frame, textvariable=var, values=["0", "1"], width=5).grid(row=2, column=i, padx=5, pady=5)

    def create_buttons_tab(self):
        buttons_frame = ttk.Frame(self.notebook, padding="10")
        self.notebook.add(buttons_frame, text="Buttons")
        
        ttk.Label(buttons_frame, text="Set the correct button sequence (1-6):").grid(row=0, column=0, columnspan=6, sticky=tk.W, padx=5, pady=5)
        
        self.button_vars = []
        for i in range(6):
            var = tk.StringVar()
            self.button_vars.append(var)
            ttk.Label(buttons_frame, text=f"Press {i+1}:").grid(row=1, column=i, padx=5, pady=5)
            ttk.Combobox(buttons_frame, textvariable=var, values=["1", "2", "3", "4", "5", "6"], width=5).grid(row=2, column=i, padx=5, pady=5)

    def create_pots_tab(self):
        pots_frame = ttk.Frame(self.notebook, padding="10")
        self.notebook.add(pots_frame, text="Potentiometers")
        
        ttk.Label(pots_frame, text="Set the correct potentiometer values (0-1023):").grid(row=0, column=0, columnspan=6, sticky=tk.W, padx=5, pady=5)
        
        self.pot_vars = []
        for i in range(6):
            var = tk.StringVar()
            self.pot_vars.append(var)
            ttk.Label(pots_frame, text=f"Pot {i+1}:").grid(row=1, column=i, padx=5, pady=5)
            ttk.Entry(pots_frame, textvariable=var, width=6).grid(row=2, column=i, padx=5, pady=5)
        
        # Add a slider for visual representation and easier setting
        ttk.Label(pots_frame, text="Visual Slider (updates entry above):").grid(row=3, column=0, columnspan=6, sticky=tk.W, padx=5, pady=10)
        
        for i in range(6):
            slider = ttk.Scale(pots_frame, from_=0, to=1023, orient=tk.HORIZONTAL, length=100)
            slider.grid(row=4, column=i, padx=5, pady=5)
            # Link slider to entry
            slider.config(command=lambda value, idx=i: self.pot_vars[idx].set(str(int(float(value)))))

    def create_jacks_tab(self):
        jacks_frame = ttk.Frame(self.notebook, padding="10")
        self.notebook.add(jacks_frame, text="Jack Connections")
        
        ttk.Label(jacks_frame, text="Set the correct jack connections (pairs of jacks 0-15):").grid(row=0, column=0, columnspan=4, sticky=tk.W, padx=5, pady=5)
        
        self.jack_vars = []
        for i in range(8):  # 8 pairs of connections
            jack1_var = tk.StringVar()
            jack2_var = tk.StringVar()
            self.jack_vars.append((jack1_var, jack2_var))
        
            ttk.Label(jacks_frame, text=f"Pair {i+1}:").grid(row=i+1, column=0, padx=5, pady=5, sticky=tk.W)
            ttk.Combobox(jacks_frame, textvariable=jack1_var, values=[str(j) for j in range(16)], width=5).grid(row=i+1, column=1, padx=5, pady=5)
            ttk.Label(jacks_frame, text="connected to").grid(row=i+1, column=2, padx=5, pady=5)
            ttk.Combobox(jacks_frame, textvariable=jack2_var, values=[str(j) for j in range(16)], width=5).grid(row=i+1, column=3, padx=5, pady=5)

    def create_keypad_tab(self):
        keypad_frame = ttk.Frame(self.notebook, padding="10")
        self.notebook.add(keypad_frame, text="Keypad Code")
        
        ttk.Label(keypad_frame, text="Set the correct keypad code:").grid(row=0, column=0, sticky=tk.W, padx=5, pady=5)
        
        ttk.Entry(keypad_frame, textvariable=self.keypad_var, width=20).grid(row=0, column=1, padx=5, pady=5, sticky=tk.W)
        
        ttk.Label(keypad_frame, text="Note: Code should only contain digits 0-9 and keys A-D, *, #").grid(row=1, column=0, columnspan=2, sticky=tk.W, padx=5, pady=5)
        
        # Add a visual keypad for easier input
        keypad_visual_frame = ttk.Frame(keypad_frame, padding="10")
        keypad_visual_frame.grid(row=2, column=0, columnspan=2, pady=10)
        
        keypad_layout = [
            ['1', '2', '3', 'A'],
            ['4', '5', '6', 'B'],
            ['7', '8', '9', 'C'],
            ['*', '0', '#', 'D']
        ]
        
        for i, row in enumerate(keypad_layout):
            for j, key in enumerate(row):
                ttk.Button(keypad_visual_frame, text=key, width=3,
                          command=lambda k=key: self.keypad_var.set(self.keypad_var.get() + k)
                          ).grid(row=i, column=j, padx=2, pady=2)
        
        # Add clear button
        ttk.Button(keypad_visual_frame, text="Clear", 
                  command=lambda: self.keypad_var.set("")
                  ).grid(row=4, column=0, columnspan=4, pady=5)

if __name__ == "__main__":
    root = tk.Tk()
    app = KeypadGameConfigurator(root)
    root.mainloop()
