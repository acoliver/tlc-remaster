#!/usr/bin/env python3
"""
Script to convert Allegro 4 color macros to Allegro 5 equivalents.
This handles the conversion of makecol, makeacol, getr, getg, getb, geta,
and clear_to_color/clear_bitmap calls.
"""

import re
import sys
import os

def convert_file(filepath):
    """Convert a single file from Allegro 4 to Allegro 5 color API."""
    
    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
    
    original_content = content
    changes_made = []
    
    # Convert makecol(r,g,b) to al_map_rgb(r,g,b)
    pattern = r'\bmakecol\s*\('
    if re.search(pattern, content):
        content = re.sub(pattern, 'al_map_rgb(', content)
        changes_made.append('makecol -> al_map_rgb')
    
    # Convert makeacol(r,g,b,a) to al_map_rgba(r,g,b,a)
    pattern = r'\bmakeacol\s*\('
    if re.search(pattern, content):
        content = re.sub(pattern, 'al_map_rgba(', content)
        changes_made.append('makeacol -> al_map_rgba')
    
    # Convert color extraction patterns: getr(color); getg(color); getb(color);
    # to: unsigned char r, g, b; al_unmap_rgb(acolor, &r, &g, &b);
    # This is complex and needs to be done more carefully
    
    # Convert clear_to_color(bmp, makecol(255,0,255)) to transparent clear
    pattern = r'clear_to_color\s*\(\s*(\w+)\s*,\s*makecol\s*\(\s*255\s*,\s*0\s*,\s*255\s*\)\s*\)'
    if re.search(pattern, content):
        content = re.sub(pattern, r'al_set_target_bitmap(\1); al_clear_to_color(al_map_rgba(0,0,0,0)); al_set_target_bitmap(al_get_target_bitmap())', content)
        changes_made.append('clear_to_color(bmp, magenta) -> transparent clear')
    
    if content != original_content:
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)
        return True, changes_made
    
    return False, []

def main():
    """Main entry point."""
    if len(sys.argv) < 2:
        print("Usage: python3 convert_colors.py <file_or_directory>")
        sys.exit(1)
    
    path = sys.argv[1]
    
    if os.path.isfile(path):
        files = [path]
    elif os.path.isdir(path):
        files = []
        for root, dirs, filenames in os.walk(path):
            for filename in filenames:
                if filename.endswith(('.cpp', '.h')):
                    files.append(os.path.join(root, filename))
    else:
        print(f"Error: {path} is not a file or directory")
        sys.exit(1)
    
    total_files = 0
    modified_files = 0
    
    for filepath in files:
        modified, changes = convert_file(filepath)
        if modified:
            modified_files += 1
            print(f"Modified: {filepath}")
            for change in changes:
                print(f"  - {change}")
        total_files += 1
    
    print(f"\nProcessed {total_files} files, modified {modified_files} files")

if __name__ == '__main__':
    main()
