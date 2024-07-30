the-button: button.c
	gcc -o the-button button.c
install: the-button
	-killall the-button
	install the-button ~/.local/bin/the-button
	install the-button.service ~/.config/systemd/user/the-button.service
	systemctl --user daemon-reload
	echo "You may want to start the service"

install-rules:
	install 50-button.rules /etc/udev/rules.d/50-button.rules
