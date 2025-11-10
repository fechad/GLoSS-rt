import gloss as m

output = m.printHelloWorld()
print(output)

version = m.getVersion()
print(f"Current version is: {version}")

# Initialize
antenna_file = "antenna_topology/videotron.csv"
data_path = "data/montreal/montreal.tif"
data_mnt_path = "data/montreal/montreal_MNT.tif"

m.initialize(antenna_file, data_path, data_mnt_path)
print("Initialization complete")

# Compute
results = m.compute()
print(f"Computation complete. Processed {len(results)} antennas")

# Optionally save results
m.saveResults(results)
print("Results saved")