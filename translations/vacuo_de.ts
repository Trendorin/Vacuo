<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="de_DE" sourcelanguage="en_US">
  <context>
    <name>QObject</name>
    <message><source>Vacuo cannot run as root</source><translation>Vacuo kann nicht als root ausgeführt werden</translation></message>
    <message><source>Run Vacuo as your normal user. System cleanup is requested separately through PolicyKit.</source><translation>Vacuo als normaler Benutzer starten. Die Systembereinigung wird separat über PolicyKit angefordert.</translation></message>
  </context>
  <context>
    <name>SettingsDialog</name>
    <message><source>Settings</source><translation>Einstellungen</translation></message>
    <message><source>Interface</source><translation>Oberfläche</translation></message>
    <message><source>System language</source><translation>Systemsprache</translation></message>
    <message><source>Language:</source><translation>Sprache:</translation></message>
    <message><source>The interface language changes immediately after saving.</source><translation>Die Oberflächensprache wird direkt nach dem Speichern geändert.</translation></message>
    <message><source>Cancel</source><translation>Abbrechen</translation></message>
    <message><source>Save</source><translation>Speichern</translation></message>
  </context>
  <context>
    <name>ScanModel</name>
    <message><source>User</source><translation>Benutzer</translation></message>
    <message><source>System</source><translation>System</translation></message>
    <message><source>Low</source><translation>Niedrig</translation></message>
    <message><source>Moderate</source><translation>Mittel</translation></message>
    <message><source>Elevated</source><translation>Erhöht</translation></message>
    <message><source>Unknown</source><translation>Unbekannt</translation></message>
    <message><source>Not present</source><translation>Nicht vorhanden</translation></message>
    <message><source>Partial scan</source><translation>Teilweise geprüft</translation></message>
    <message><source>Limited access</source><translation>Eingeschränkter Zugriff</translation></message>
    <message><source>Clean</source><translation>Leer</translation></message>
    <message><source>Ready</source><translation>Bereit</translation></message>
    <message><source>Category</source><translation>Kategorie</translation></message>
    <message><source>Scope</source><translation>Bereich</translation></message>
    <message><source>Risk</source><translation>Risiko</translation></message>
    <message><source>Size</source><translation>Größe</translation></message>
    <message><source>Items</source><translation>Objekte</translation></message>
    <message><source>Status</source><translation>Status</translation></message>
  </context>
  <context>
    <name>MainWindow</name>
    <message><source>Vacuo — Linux cache cleaner</source><translation>Vacuo — Linux-Cache-Bereinigung</translation></message>
    <message><source>Vacuo</source><translation>Vacuo</translation></message>
    <message><source>Vacuo application title</source><translation>Anwendungstitel von Vacuo</translation></message>
    <message><source>Native Linux cache control</source><translation>Native Linux-Cache-Verwaltung</translation></message>
    <message><source>Scanning the system…</source><translation>System wird geprüft…</translation></message>
    <message><source>Scan summary</source><translation>Prüfzusammenfassung</translation></message>
    <message><source>Cleanup categories</source><translation>Bereinigungskategorien</translation></message>
    <message><source>Details</source><translation>Details</translation></message>
    <message><source>Selected category details</source><translation>Details der ausgewählten Kategorie</translation></message>
    <message><source>Nothing selected</source><translation>Nichts ausgewählt</translation></message>
    <message><source>Scan again</source><translation>Erneut prüfen</translation></message>
    <message><source>Scan supported cache locations without changing files</source><translation>Unterstützte Cache-Orte prüfen, ohne Dateien zu verändern</translation></message>
    <message><source>Clean selected</source><translation>Auswahl bereinigen</translation></message>
    <message><source>Review and permanently clean selected cache categories</source><translation>Ausgewählte Cache-Kategorien prüfen und dauerhaft bereinigen</translation></message>
    <message><source>%1 · %2 · %3</source><translation>%1 · %2 · %3</translation></message>
    <message><source>Detected package manager: %1</source><translation>Erkannter Paketmanager: %1</translation></message>
    <message><source>Scanning supported cache locations…</source><translation>Unterstützte Cache-Orte werden geprüft…</translation></message>
    <message><source>Scanning is read-only. No files are changed.</source><translation>Die Prüfung erfolgt schreibgeschützt. Es werden keine Dateien verändert.</translation></message>
    <message><source>Found %1 across %2 removable entries. Review each category before cleaning.</source><translation>%1 in %2 entfernbaren Objekten gefunden. Jede Kategorie vor der Bereinigung prüfen.</translation></message>
    <message><source>Selected: %1 in %2 categories</source><translation>Ausgewählt: %1 in %2 Kategorien</translation></message>
    <message><source>Select a category to see exactly what Vacuo will clean.</source><translation>Eine Kategorie auswählen, um genau zu sehen, was Vacuo bereinigt.</translation></message>
    <message><source>Note:</source><translation>Hinweis:</translation></message>
    <message><source>Permanently clean %1 from %2 selected categories?</source><translation>%1 aus %2 ausgewählten Kategorien dauerhaft bereinigen?</translation></message>
    <message><source>System categories require a PolicyKit authentication prompt.</source><translation>Systemkategorien erfordern eine Authentifizierung über PolicyKit.</translation></message>
    <message><source>At least one selected category is not reversible.</source><translation>Mindestens eine ausgewählte Kategorie kann nicht wiederhergestellt werden.</translation></message>
    <message><source>Confirm cleanup</source><translation>Bereinigung bestätigen</translation></message>
    <message><source>Clean</source><translation>Bereinigen</translation></message>
    <message><source>Cancel</source><translation>Abbrechen</translation></message>
    <message><source>Cleaning selected categories…</source><translation>Ausgewählte Kategorien werden bereinigt…</translation></message>
    <message><source>Cleanup complete</source><translation>Bereinigung abgeschlossen</translation></message>
    <message><source>Freed %1 and removed %2 entries.</source><translation>%1 freigegeben und %2 Objekte entfernt.</translation></message>
    <message><source>Cleanup completed with issues</source><translation>Bereinigung mit Problemen abgeschlossen</translation></message>
    <message><source>Freed %1. Some categories could not be completed.</source><translation>%1 freigegeben. Einige Kategorien konnten nicht vollständig bereinigt werden.</translation></message>
    <message><source>Vacuo security model</source><translation>Sicherheitsmodell von Vacuo</translation></message>
    <message><source>Vacuo never follows symbolic links while cleaning. User targets come from a compiled allow-list. The graphical application never runs as root; fixed system actions are delegated to a small PolicyKit helper that accepts action identifiers, not commands or paths. Vacuo has no telemetry and makes no network requests.</source><translation>Vacuo folgt bei der Bereinigung niemals symbolischen Verknüpfungen. Benutzerziele stammen aus einer fest eingebauten Positivliste. Die grafische Anwendung läuft niemals als root; feste Systemaktionen werden an einen kleinen PolicyKit-Helfer delegiert, der Aktionskennungen statt Befehlen oder Pfaden akzeptiert. Vacuo enthält keine Telemetrie und führt keine Netzwerkanfragen aus.</translation></message>
    <message><source>About Vacuo</source><translation>Über Vacuo</translation></message>
    <message><source>&lt;h3&gt;Vacuo %1&lt;/h3&gt;&lt;p&gt;A native C++20 and Qt 6 Widgets cache cleaner for Linux.&lt;/p&gt;&lt;p&gt;Copyright © 2026 Trendorin. Licensed under GPL-3.0-or-later.&lt;/p&gt;</source><translation>&lt;h3&gt;Vacuo %1&lt;/h3&gt;&lt;p&gt;Eine native Cache-Bereinigung für Linux mit C++20 und Qt 6 Widgets.&lt;/p&gt;&lt;p&gt;Copyright © 2026 Trendorin. Lizenziert unter GPL-3.0-or-later.&lt;/p&gt;</translation></message>
    <message><source>&amp;File</source><translation>&amp;Datei</translation></message>
    <message><source>&amp;Edit</source><translation>&amp;Bearbeiten</translation></message>
    <message><source>&amp;Help</source><translation>&amp;Hilfe</translation></message>
    <message><source>&amp;Scan again</source><translation>&amp;Erneut prüfen</translation></message>
    <message><source>&amp;Quit</source><translation>&amp;Beenden</translation></message>
    <message><source>Settings…</source><translation>Einstellungen…</translation></message>
    <message><source>Security model</source><translation>Sicherheitsmodell</translation></message>
    <message><source>Vacuo is busy</source><translation>Vacuo ist beschäftigt</translation></message>
    <message><source>Wait for the current operation to finish.</source><translation>Warten, bis der aktuelle Vorgang abgeschlossen ist.</translation></message>
  </context>
  <context>
    <name>VacuoData</name>
    <message><source>Application cache</source><translation>Anwendungs-Cache</translation></message>
    <message><source>General XDG cache data, excluding the separately listed browser, developer and thumbnail groups.</source><translation>Allgemeine XDG-Cache-Daten ohne die separat aufgeführten Browser-, Entwickler- und Vorschaubildgruppen.</translation></message>
    <message><source>Applications may start more slowly once while their caches are rebuilt.</source><translation>Anwendungen können beim nächsten Start langsamer sein, während ihre Caches neu aufgebaut werden.</translation></message>
    <message><source>Thumbnail cache</source><translation>Vorschaubild-Cache</translation></message>
    <message><source>File-manager and desktop thumbnail previews.</source><translation>Vorschaubilder von Dateimanagern und Desktop-Umgebungen.</translation></message>
    <message><source>Previews are recreated automatically when folders are opened.</source><translation>Vorschaubilder werden beim Öffnen von Ordnern automatisch neu erstellt.</translation></message>
    <message><source>Browser cache</source><translation>Browser-Cache</translation></message>
    <message><source>Rebuildable HTTP and media caches for supported Firefox- and Chromium-based browsers.</source><translation>Neu erstellbare HTTP- und Medien-Caches unterstützter Firefox- und Chromium-basierter Browser.</translation></message>
    <message><source>Close browsers first. This does not remove profiles, history, cookies, passwords or extensions.</source><translation>Browser zuerst schließen. Profile, Verlauf, Cookies, Passwörter und Erweiterungen werden nicht entfernt.</translation></message>
    <message><source>Developer cache</source><translation>Entwickler-Cache</translation></message>
    <message><source>Package downloads and build caches from npm, Gradle, Cargo, pip, uv, Go and supported IDEs.</source><translation>Paketdownloads und Build-Caches von npm, Gradle, Cargo, pip, uv, Go und unterstützten IDEs.</translation></message>
    <message><source>The next dependency install or build can take longer and may download data again.</source><translation>Die nächste Installation von Abhängigkeiten oder der nächste Build kann länger dauern und Daten erneut herunterladen.</translation></message>
    <message><source>Flatpak application cache</source><translation>Flatpak-Anwendungs-Cache</translation></message>
    <message><source>Only each application's cache directory under ~/.var/app; application data and settings stay intact.</source><translation>Nur das Cache-Verzeichnis jeder Anwendung unter ~/.var/app; Anwendungsdaten und Einstellungen bleiben erhalten.</translation></message>
    <message><source>Close Flatpak applications before cleaning their caches.</source><translation>Flatpak-Anwendungen vor der Bereinigung ihrer Caches schließen.</translation></message>
    <message><source>Steam shader and web cache</source><translation>Steam-Shader- und Web-Cache</translation></message>
    <message><source>Rebuildable Steam shader cache and embedded web-interface cache.</source><translation>Neu erstellbarer Steam-Shader-Cache und Cache der eingebetteten Weboberfläche.</translation></message>
    <message><source>Shaders can be compiled again and a game may stutter on its next launch.</source><translation>Shader können erneut kompiliert werden und ein Spiel kann beim nächsten Start ruckeln.</translation></message>
    <message><source>Trash</source><translation>Papierkorb</translation></message>
    <message><source>Freedesktop Trash files and their metadata.</source><translation>Dateien im Freedesktop-Papierkorb und deren Metadaten.</translation></message>
    <message><source>This permanently removes every item currently in Trash.</source><translation>Alle derzeit im Papierkorb befindlichen Objekte werden dauerhaft entfernt.</translation></message>
    <message><source>Package cache</source><translation>Paket-Cache</translation></message>
    <message><source>Cached pacman packages; two installed versions are retained.</source><translation>Zwischengespeicherte pacman-Pakete; zwei installierte Versionen bleiben erhalten.</translation></message>
    <message><source>Uses paccache through the restricted Vacuo helper.</source><translation>Verwendet paccache über den eingeschränkten Vacuo-Helfer.</translation></message>
    <message><source>Downloaded APT package archives.</source><translation>Heruntergeladene APT-Paketarchive.</translation></message>
    <message><source>Uses apt-get clean through the restricted Vacuo helper.</source><translation>Verwendet apt-get clean über den eingeschränkten Vacuo-Helfer.</translation></message>
    <message><source>DNF metadata and downloaded packages.</source><translation>DNF-Metadaten und heruntergeladene Pakete.</translation></message>
    <message><source>Uses dnf/dnf5 clean all through the restricted Vacuo helper.</source><translation>Verwendet dnf/dnf5 clean all über den eingeschränkten Vacuo-Helfer.</translation></message>
    <message><source>zypper repository metadata and downloaded packages.</source><translation>zypper-Repository-Metadaten und heruntergeladene Pakete.</translation></message>
    <message><source>Uses zypper clean --all through the restricted Vacuo helper.</source><translation>Verwendet zypper clean --all über den eingeschränkten Vacuo-Helfer.</translation></message>
    <message><source>Downloaded APK packages.</source><translation>Heruntergeladene APK-Pakete.</translation></message>
    <message><source>Uses apk cache clean through the restricted Vacuo helper.</source><translation>Verwendet apk cache clean über den eingeschränkten Vacuo-Helfer.</translation></message>
    <message><source>Archived system journal</source><translation>Archiviertes Systemjournal</translation></message>
    <message><source>Archived systemd journal entries older than 14 days.</source><translation>Archivierte systemd-Journaleinträge, die älter als 14 Tage sind.</translation></message>
    <message><source>Active journal files are retained; journalctl decides the exact eligible set.</source><translation>Aktive Journaldateien bleiben erhalten; journalctl bestimmt die genaue Auswahl.</translation></message>
    <message><source>System crash dumps</source><translation>System-Absturzabbilder</translation></message>
    <message><source>Stored systemd-coredump files.</source><translation>Gespeicherte systemd-coredump-Dateien.</translation></message>
    <message><source>Crash dumps used for debugging cannot be restored after cleaning.</source><translation>Zur Fehlersuche verwendete Absturzabbilder können nach der Bereinigung nicht wiederhergestellt werden.</translation></message>
    <message><source>Snap download cache</source><translation>Snap-Download-Cache</translation></message>
    <message><source>Downloaded Snap package cache. Installed snaps and revisions are not touched.</source><translation>Cache heruntergeladener Snap-Pakete. Installierte Snaps und Revisionen bleiben unberührt.</translation></message>
    <message><source>The same packages may need to be downloaded again.</source><translation>Dieselben Pakete müssen möglicherweise erneut heruntergeladen werden.</translation></message>
    <message><source>Scan limit reached; displayed size is a lower bound.</source><translation>Prüflimit erreicht; die angezeigte Größe ist eine Untergrenze.</translation></message>
    <message><source>Some entries could not be read.</source><translation>Einige Objekte konnten nicht gelesen werden.</translation></message>
    <message><source>Not present on this system.</source><translation>Auf diesem System nicht vorhanden.</translation></message>
    <message><source>Already empty.</source><translation>Bereits leer.</translation></message>
    <message><source>Cleanup completed.</source><translation>Bereinigung abgeschlossen.</translation></message>
    <message><source>Unknown cleanup rule.</source><translation>Unbekannte Bereinigungsregel.</translation></message>
    <message><source>Vacuo's installed PolicyKit helper is unavailable.</source><translation>Der installierte PolicyKit-Helfer von Vacuo ist nicht verfügbar.</translation></message>
    <message><source>PolicyKit helper timed out.</source><translation>Zeitüberschreitung beim PolicyKit-Helfer.</translation></message>
    <message><source>PolicyKit authorization was cancelled or the helper failed.</source><translation>Die PolicyKit-Autorisierung wurde abgebrochen oder der Helfer ist fehlgeschlagen.</translation></message>
    <message><source>paccache is unavailable; install pacman-contrib. Vacuo will not fall back to pacman -Scc.</source><translation>paccache ist nicht verfügbar; pacman-contrib installieren. Vacuo weicht nicht auf pacman -Scc aus.</translation></message>
    <message><source>No allow-listed package-cache command is available for this distribution.</source><translation>Für diese Distribution ist kein zugelassener Befehl zur Bereinigung des Paket-Caches verfügbar.</translation></message>
    <message><source>journalctl is not installed.</source><translation>journalctl ist nicht installiert.</translation></message>
  </context>
</TS>
