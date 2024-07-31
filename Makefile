the-button: button.c button-light.c
	gcc -o the-button button.c
	gcc -o button-light button-light.c -lusb-1.0
install: the-button
	-killall the-button
	install the-button ~/.local/bin/the-button
	install button-light ~/.local/bin/button-light
	install the-button.service ~/.config/systemd/user/the-button.service
	install button-pressed ~/.local/bin/button-pressed
	install start-scanning ~/.local/bin/start-scanning
	systemctl --user daemon-reload
	echo "You may want to start the service"

install-rules:
	install 50-button.rules /etc/udev/rules.d/50-button.rules
	udevadm control --reload-rules
	udevadm trigger
