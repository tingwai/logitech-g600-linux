import subprocess

import sys

# bytestring used because console output is also bytestring, minor optimization
CUSTOM_BINDINGS = {
    b'terminal': {
        'G11': 'xdotool key ctrl+Page_Up',
        'G14': 'xdotool key ctrl+Page_Down',
    },
    b'firefox': {},
    b'idea': {
        'G9': 'xdotool key ctrl+shift+F4',
        'G11': 'xdotool key alt+Left',
        'G12': 'xdotool key ctrl+F4',
        'G14': 'xdotool key alt+Right',
        'G-shift + G10': 'xdotool key ctrl+Home',
        'G-shift + G13': 'xdotool key ctrl+End',
    },
    # b'sublime': {},
    b'spotify': {
        'G11': 'xdotool key alt+Left',
        'G14': 'xdotool key alt+Right',
    },
    b'pycharm': {
        'G9': 'xdotool key ctrl+shift+F4',
        'G11': 'xdotool key alt+Left',
        'G12': 'xdotool key ctrl+F4',
        'G14': 'xdotool key alt+Right',
        'G-shift + G10': 'xdotool key ctrl+Home',
        'G-shift + G13': 'xdotool key ctrl+End',
    },
    # b'robo3t': {},
    # b'chrome': {},
    # b'chromium': {},
}
for program, bindings in CUSTOM_BINDINGS.items():
    # convert key binding into array of strings, for subprocess.run to consume
    # eg. ['xdotool', 'key', 'ctrl+Page_Up']
    bindings.update((key, val.split(' ')) for key, val in bindings.items())

DEFAULT_BINDINGS = {
    'G9': 'xdotool key ctrl+shift+t',
    'G10': 'xdotool key Page_Up',
    'G11': 'xdotool key ctrl+shift+Tab',
    'G12': 'xdotool key ctrl+w',
    'G13': 'xdotool key Page_Down',
    'G14': 'xdotool key ctrl+Tab',
    'G15': 'xdotool key alt+space key n',
    'G16': 'xdotool key ctrl+alt+Up',
    'G17': 'xdotool key ctrl+alt+Left',
    'G18': 'xdotool key alt+F4',
    'G19': 'xdotool key ctrl+alt+Down',
    'G20': 'xdotool key ctrl+alt+Right',
    'G-shift + G9': 'xdotool key alt+9',
    'G-shift + G10': 'xdotool key Home',
    'G-shift + G11': 'xdotool key ctrl+shift+Page_Up',
    'G-shift + G12': 'xdotool key ctrl+r',
    'G-shift + G13': 'xdotool key End',
    'G-shift + G14': 'xdotool key ctrl+shift+Page_Down',
    'G-shift + G15': 'playerctl play-pause spotify',
    'G-shift + G16': 'xdotool key Escape',
    'G-shift + G17': 'amixer -D pulse sset Master 2%+',
    'G-shift + G18': 'playerctl next spotify',
    'G-shift + G19': 'xdotool key alt+Tab sleep 0.1 key alt+Tab',
    'G-shift + G20': 'amixer -D pulse sset Master 2%-',
}
# convert key binding into array of strings, for subprocess.run to consume
# eg. ['xdotool', 'key', 'ctrl+Page_Up']
DEFAULT_BINDINGS.update((key, val.split(' ')) for key, val in DEFAULT_BINDINGS.items())


def main(mouse_button):
    # get active window and number
    active_window = subprocess.run(['xdotool', 'getactivewindow'], stdout=subprocess.PIPE)
    active_num = active_window.stdout.strip()

    # run custom binding if active window is customized
    for program in CUSTOM_BINDINGS.keys():
        program_list = subprocess.run(['xdotool', 'search', '--class', program], stdout=subprocess.PIPE)
        program_nums = program_list.stdout

        if active_num in program_nums:
            run_custom(program, mouse_button)
            return

    run_default(mouse_button)


def run_default(mouse_button):
    subprocess.run(DEFAULT_BINDINGS[mouse_button])
    print_action(b'default', mouse_button, DEFAULT_BINDINGS[mouse_button])


def run_custom(program, mouse_button):
    if mouse_button not in CUSTOM_BINDINGS[program]:
        run_default(mouse_button)
    else:
        subprocess.run(CUSTOM_BINDINGS[program][mouse_button])
        print_action(program, mouse_button, CUSTOM_BINDINGS[program][mouse_button])


def print_action(program, mouse_button, binding):
    print(f'{program.upper().decode()}: {mouse_button}: {" ".join(binding)}\n')


if __name__ == '__main__':
    mouse_button = sys.argv[1]
    main(mouse_button)
