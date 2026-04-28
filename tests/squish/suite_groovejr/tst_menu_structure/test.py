def main():
    source(findFile("scripts", "common.py"))

    start_groovejr()

    file_titles = menu_action_titles(file_menu())
    test.compare(file_titles, ["Render"], "File menu exposes the Render action.")

    music_titles = menu_action_titles(music_menu())
    test.compare(music_titles, ["Add Folder To Library"], "Music menu exposes the library import action.")
