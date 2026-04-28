import re
import sys

def parse_report(filename):
    try:
        with open(filename, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
    except Exception as e:
        print(f"Error reading {filename}: {e}")
        return
        
    trs = re.findall(r'(?s)<TR>(.*?)</TR>', content)
    for tr in trs:
        if 'BGCOLOR="RED"' in tr or 'BGCOLOR="YELLOW"' in tr:
            match = re.search(r'<A HREF="([^"]+)"', tr)
            module = match.group(1).split('.html')[0] if match else 'Unknown'
            cleaned = re.sub(r'<[^>]+>', ' ', tr).replace('&nbsp;', ' ').strip()
            print(f'Module: {module}')
            print(' '.join(cleaned.split()))

print('--- CLI ---')
parse_report('cccc_cli_report.html')
print('--- GUI ---')
parse_report('cccc_gui_report.html')
