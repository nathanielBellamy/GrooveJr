def main_window():
    return {
        "type": "QMainWindow",
        "visible": "1",
        "windowTitle": "GrooveJr",
    }


def named_object(object_name):
    return {
        "objectName": object_name,
    }


def menu_bar():
    return named_object("mainMenuBar")


def file_menu():
    return named_object("fileMenu")


def music_menu():
    return named_object("musicMenu")


def render_action():
    return named_object("renderAction")


def add_folder_to_library_action():
    return named_object("addFolderToLibraryAction")


def files_button():
    return named_object("filesButton")


def queue_button():
    return named_object("queueButton")


def cache_button():
    return named_object("cacheButton")


def filters_header():
    return named_object("filtersHeader")


def album_header():
    return named_object("albumHeader")


def artist_header():
    return named_object("artistHeader")


def genre_header():
    return named_object("genreHeader")


def playlist_header():
    return named_object("playlistHeader")


def audio_file_table():
    return named_object("audioFileTableView")


def queue_table():
    return named_object("queueTableView")


def cache_table():
    return named_object("cacheTableView")
