#!/usr/bin/python3

# Convert text file to HTML
#     '* ...' is a title (output file)
#     '^  <...>' is file (included)
#     '^  - ' is list
#     '^  [0-9]. ' is enum
#     '^  ' is code
#     '^ *$' is end of code
#     Everything else is text

import fileinput
import re
import sys

# Prolog
outFileNumber = 0
inCode = False
inList = False
inEnum = False
inText = False
stdoutSave = sys.stdout

# Function to start block
def startCode(isC):
    global inCode
    if isC:
        codeClass = 'lang-c'
    else:
        codeClass = 'nocode'
    if not inCode:
        print(f'<pre class="prettyprint"><code class="{codeClass}">', end='')
        inCode = True

def startList():
    global inList
    if not inList:
        print(f'<ul>', end='')
        inList = True

def startEnum():
    global inEnum
    if not inEnum:
        print(f'<ol>', end='')
        inEnum = True

def startText():
    global inText
    if not inText:
        print(f'<p>', end='')
        inText = True

def endCode():
    global inCode
    if inCode:
        print('</code></pre>')
        print()
        inCode = False

def endList():
    global inList
    if inList:
        print('</ul>')
        print()
        inList = False

def endEnum():
    global inEnum
    if inEnum:
        print('</ol>')
        print()
        inEnum = False

def endText():
    global inText
    if inText:
        print('</p>')
        print()
        inText = False

# Input from argument or stdin
for line in fileinput.input(sys.argv[1:]):

    # Ignore lines
    if re.search(r'^[*] ?[=-]{10}', line):
        continue

    # Check end of document
    if re.search(r'^[*]? ?Local Variables', line):
        break

    # Check header
    m = re.search(r'^[*] (.*)$', line)
    if m:
        # Close previous block
        endCode()
        endList()
        endEnum()
        endText()

        # New header
        fileName = m.groups()[0] + '.html'
        outFileNumber += 1
        print(f'{outFileNumber}: {fileName}', file=sys.stderr)
        sys.stdout = open(fileName, 'w')
        print('<span style="font-size: medium;">', end='')
        continue

    # Check end of code
    m = re.search(r'^[ \t]*$', line)
    if m:
        endCode()
        endList()
        endEnum()
        endText()
        continue

    # Check code include
    m = re.search(r'^  <(.*)> *$', line)
    if m:
        fileName = m.groups()[0]
        print(f'  {fileName}', file=sys.stderr)
        isC = re.search(r'\.c$', fileName)
        startCode(isC)
        with open(fileName, 'r') as f:
            for code in f:
                code = code.expandtabs()
                code = code.replace('<', '&lt;')
                print(code, end='')
        continue

    # Expand forbidden characters
    line = line.expandtabs()
    line = line.replace('<', '&lt;')

    # Check HTTP link
    m = re.search(r'^  (http.*)$', line)
    if m:
        url = m.groups()[0]
        print(f'  <p><a href="{url}">{url}</a></p>')
        continue

    # Check itemize
    m = re.search(r'^  - (.*)$', line)
    if m:
        startList()
        code = m.groups()[0]
        print(f'  <li> {code}')
        continue

    # Check enumeration
    m = re.search(r'^  [0-9][.] (.*)$', line)
    if m:
        startEnum()
        code = m.groups()[0]
        print(f'  <li> {code}')
        continue

    # Check code
    m = re.search(r'^  (.*)$', line)
    if m:
        startCode(isC=False)
        code = m.groups()[0]
        print(code)
        continue

    # All the other case
    startText()
    print(line.rstrip())

# Epilog
endCode()
endList()
endEnum()
endText()
sys.stdout = stdoutSave

# Local Variables:
# compile-command: "python3 TextToHtml.py LearnByDoingLinuxDriver"
# End:
