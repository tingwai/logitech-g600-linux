import re
import subprocess
import sys

"""
+-------+-------+-------+-------+
|  G11  |  G14  |  G17  |  G20  |
+-------+-------+-------+-------+
|  G10  |  G13  |  G16  |  G19  |
+-------+-------+-------+-------+
|  G9   |  G12  |  G15  |  G18  |
+-------+-------+-------+-------+
"""

# key names are the instance names from WM_CLASS (ie. first value from `xprop WM_CLASS`)
# values are a dictionary mapping mouse button coming from driving and its corresponding command
CUSTOM_BINDINGS = {
    'Navigator': {},  # firefox, no custom hotkeys yet
    'jetbrains-goland': {
        'G9': 'xdotool key ctrl+shift+F4',            # restore tab
        'G11': 'xdotool key alt+Left',                # go to left tab
        'G12': 'xdotool key ctrl+F4',                 # close tab
        'G14': 'xdotool key alt+Right',               # go to right tab
        'G-shift + G9': 'xdotool key alt+shift+Up',   # previous change
        'G-shift + G10': 'xdotool key ctrl+Home',     # top of file
        'G-shift + G11': 'xdotool key alt+Up',        # previous method
        'G-shift + G12': 'xdotool key alt+shift+Down',# next change
        'G-shift + G13': 'xdotool key ctrl+End',      # bottom of file
        'G-shift + G14': 'xdotool key alt+Down',      # next method
        'G-shift + G16': 'xdotool key shift+Escape',  # hide window
    },
    'idea': {
        # 'G9': 'xdotool key ctrl+shift+F4',
        # 'G11': 'xdotool key alt+Left',
        # 'G12': 'xdotool key ctrl+F4',
        # 'G14': 'xdotool key alt+Right',
        'G-shift + G10': 'xdotool key ctrl+Home',
        'G-shift + G13': 'xdotool key ctrl+End',
        'G-shift + G16': 'xdotool key shift+Escape',
    },
    'gnome-terminal-server': {
        'G10': 'xdotool key shift+Page_Up',
        # 'G11': 'xdotool key ctrl+Page_Up',
        'G11': 'true',  # no-op
        'G13': 'xdotool key shift+Page_Down',
        # 'G14': 'xdotool key ctrl+Page_Down',
        'G14': 'true',  # no-op
        'G-shift + G10': 'xdotool key shift+Home',
        'G-shift + G13': 'xdotool key shift+End',
    },
    'sublime_text': {
        'G-shift + G10': 'xdotool key ctrl+Home',     # top of file
        'G-shift + G13': 'xdotool key ctrl+End',      # bottom of file
    },
    'spotify': {
        'G11': 'xdotool key alt+Left',   # back page
        'G14': 'xdotool key alt+Right',  # forward page
    },
    'pycharm': {
        'G9': 'xdotool key ctrl+shift+F4',
        'G11': 'xdotool key alt+Left',
        'G12': 'xdotool key ctrl+F4',
        'G14': 'xdotool key alt+Right',
        'G-shift + G10': 'xdotool key ctrl+Home',
        'G-shift + G13': 'xdotool key ctrl+End',
        'G-shift + G16': 'xdotool key shift+Escape',
    },
    # 'chrome': {},
    # 'chromium': {},
    'vlc': {
        'G9': 'xdotool key shift+Left',                # back 3 seconds
        'G10': 'xdotool key alt+Left',                 # back 10 seconds
        'G11': 'xdotool key ctrl+Left',                # back 60 seconds
        'G12': 'xdotool key shift+Right',              # forward 3 seconds
        'G13': 'xdotool key alt+Right',                # forward 10 seconds
        'G14': 'xdotool key ctrl+Right',               # forward 60 seconds
        'G-shift + G15': 'xdotool key XF86AudioPlay',  # play/pause
    }
}
for program, bindings in CUSTOM_BINDINGS.items():
    # convert key binding into array of strings, for subprocess.run to consume
    # eg. ['xdotool', 'key', 'ctrl+Page_Up']
    bindings.update((key, val.split(' ')) for key, val in bindings.items())

DEFAULT_BINDINGS = {
    'G9':            'xdotool key ctrl+shift+t',          # restore last closed tab
    'G10':           'xdotool key Page_Up',
    'G11':           'xdotool key ctrl+shift+Tab',        # previous tab/view
    'G12':           'xdotool key ctrl+w',                # close tab/view/window
    'G13':           'xdotool key Page_Down',
    'G14':           'xdotool key ctrl+Tab',              # next tab/view
    'G15':           'xdotool key alt+space key n',
    'G16':           'xdotool key Super_L+h',             # i3: focus left
    'G17':           'xdotool key Super_L+shift+Tab',     # i3: workspace left
    'G18':           'xdotool key alt+F4',
    'G19':           'xdotool key Super_L+l',             # i3: focus right
    'G20':           'xdotool key Super_L+Tab',           # i3: workspace right
    'G-shift + G9':  'xdotool key alt+9',                 # last tab (for browser)
    'G-shift + G10': 'xdotool key Home',
    'G-shift + G11': 'xdotool key ctrl+shift+Page_Up',    # move tab left/up
    'G-shift + G12': 'xdotool key ctrl+r',                # reload page
    'G-shift + G13': 'xdotool key End',
    'G-shift + G14': 'xdotool key ctrl+shift+Page_Down',  # move tab right/down
    'G-shift + G15': 'playerctl play-pause spotify',      # music: play/pause
    'G-shift + G16': 'xdotool key Escape',
    'G-shift + G17': 'amixer -D pulse sset Master 3%+',   # volume up
    'G-shift + G18': 'playerctl next spotify',            # music: next track
    'G-shift + G19': 'xdotool key Super_L+j',             # i3: focus down
    'G-shift + G20': 'amixer -D pulse sset Master 3%-',   # volume down
}
# convert key binding into array of strings, for subprocess.run to consume
# eg. ['xdotool', 'key', 'ctrl+Page_Up']
DEFAULT_BINDINGS.update((key, val.split(' ')) for key, val in DEFAULT_BINDINGS.items())

# manual override bindings since splitting on space won't work here
# TODO: needs clean up
DEFAULT_BINDINGS['G-shift + G15'] = ['python3', '-c', 'import sys; sys.stdout = open("/tmp/mypipe", "w"); print("pause")']
DEFAULT_BINDINGS['G-shift + G18'] = ['python3', '-c', 'from roku import Roku; r=Roku("10.0.0.42"); r.volume_down(); r.volume_down()']


def main(mouse_button):
    # get active window name
    active_window = subprocess.run(['xprop WM_CLASS -id $(xdotool getactivewindow)'], stdout=subprocess.PIPE, shell=True)
    active_window = active_window.stdout.strip()

    # get two strings in quotation marks
    # eg. 'WM_CLASS(STRING) = "Navigator", "Firefox"' --> ["Navigator", "Firefox"]
    instance, class_name = re.findall(r'"(.*?)"', str(active_window))

    if instance in CUSTOM_BINDINGS.keys():
        run_custom(instance, mouse_button)
    else:
        run_default(mouse_button)

    return

def run_default(mouse_button):
    subprocess.run(DEFAULT_BINDINGS[mouse_button])
    print_action('default', mouse_button, DEFAULT_BINDINGS[mouse_button])


def run_custom(program, mouse_button):
    if mouse_button not in CUSTOM_BINDINGS[program]:
        run_default(mouse_button)
    else:
        subprocess.run(CUSTOM_BINDINGS[program][mouse_button])
        print_action(program, mouse_button, CUSTOM_BINDINGS[program][mouse_button])


def print_action(program, mouse_button, binding):
    print(f'{program.upper()}: {mouse_button}: {" ".join(binding)}\n')


if __name__ == '__main__':
    mouse_button = sys.argv[1]
    main(mouse_button)
