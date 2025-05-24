import json
import pandas as pd
# Load the JSON file
with open('./smartenergyconsumption-3dc05-default-rtdb-export.json') as json_file:
    data = json.load(json_file)
# Extract data from the 'SmartEnergy' dictionary
smart_energy_data = data['SmartEnergy']
# Create a list to hold rows of data
rows = []
# Loop through the JSON data to extract timestamp and measurements
for timestamp, measurements in smart_energy_data.items():
    row = {

    'current': measurements.get('current', None),
    'power': measurements.get('power', None),
    'voltage': measurements.get('voltage', None)
    }
    rows.append(row)
# Create a DataFrame from the rows
df = pd.DataFrame(rows)
# Export the DataFrame to an Excel file
df.to_excel('./smart_energy_data.xlsx', index=False)
print("Data has been converted to Excel file successfully.")
