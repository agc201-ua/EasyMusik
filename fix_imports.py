import os
import sys

# Get the absolute path of the current directory
current_dir = os.path.dirname(os.path.abspath(__file__))
print(f"Current directory: {current_dir}")

# Add the current directory to Python's path
if current_dir not in sys.path:
    sys.path.insert(0, current_dir)
    print(f"Added {current_dir} to sys.path")

print("Python path is now:")
for p in sys.path:
    print(f"  {p}")

# Try importing the module
try:
    import MusicXMLConverter
    print(f"Successfully imported MusicXMLConverter module from {MusicXMLConverter.__file__}")
    
    # Now try importing the class
    try:
        from MusicXMLConverter import MusicXMLConverter as MXLConverter
        print("Successfully imported MusicXMLConverter class")
    except ImportError as e:
        print(f"Could not import MusicXMLConverter class: {e}")
        print("Contents of MusicXMLConverter module:", dir(MusicXMLConverter))
except ImportError as e:
    print(f"Could not import MusicXMLConverter module: {e}")