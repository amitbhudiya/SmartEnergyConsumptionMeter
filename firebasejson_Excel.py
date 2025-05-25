import json
import pandas as pd

# Open and load the JSON file containing smart energy data
with open('./smartenergyconsumption-3dc05-default-rtdb-export.json') as file:
    data = json.load(file)

# Access the 'SmartEnergy' section of the JSON structure
smart_energy_entries = data['SmartEnergy']

# Prepare a list to store each row of extracted values
records = []

# Iterate through each timestamped entry to retrieve current, power, and voltage
for timestamp, values in smart_energy_entries.items():
    record = {
        'timestamp': timestamp,
        'current': values.get('current'),
        'power': values.get('power'),
        'voltage': values.get('voltage')
    }
    records.append(record)

# Convert the list of records into a pandas DataFrame
df = pd.DataFrame(records)

# Save the DataFrame as an Excel file
df.to_excel('./smart_energy_data.xlsx', index=False)

print("Smart energy data has been successfully exported to an Excel file.")