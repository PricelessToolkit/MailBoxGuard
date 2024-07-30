## HARestAPI Configuration

- Open `LoRa_Gateway_HARestAPI.ino`
- For the `ha_pwd` go to your User Profile > Long-Lived Access Tokens > Create Token

```c
const char* ha_ip = "192.168.0.xxx";
uint16_t ha_port = 8123;
const char* ha_pwd = "HA_PASSWORD";
```

## HARestAPI

- File `configuration.yaml`

```yaml
input_boolean:
  mailbox_guard_motion:
    name: Mailbox Guard Motion
    icon: mdi:mail
  mailbox_guard_low_battery:
    name: Mailbox Guard Low Battery
    icon: mdi:battery

input_number:
  mailbox_guard_count:
    name: Mailbox Guard Count
    min: 0
    max: 255
    icon: mdi:mail
  mailbox_guard_battery:
    name: Mailbox Guard Battery
    min: 0
    max: 100
    icon: mdi:battery
  mailbox_guard_rssi:
    name: Mailbox Guard RSSI
    min: -196
    max: 63
    icon: mdi:signal
  mailbox_guard_snr:
    name: Mailbox Guard SNR
    min: -32.0
    max: 32.0
    icon: mdi:waves
```

- File `automation.yaml`

```yaml
- id: "1693702614043"
  alias: Mailbox Guard Motion
  description: ""
  trigger:
    - platform: state
      entity_id: input_boolean.mailbox_guard_motion
      to: "on"
  condition: []
  action:
    - service: notify.notify
      data:
        message: You've Got Mail!
    - service: input_number.set_value
      data:
        entity_id: input_number.mailbox_guard_count
        value: "{{ (states.input_number.mailbox_guard_count.state | int) + 1 }}"
    - service: input_boolean.turn_off
      data:
        entity_id: input_boolean.mailbox_guard_motion
  mode: single
- id: "1693703621871"
  alias: Mailbox Guard Low Battery
  description: ""
  trigger:
    - platform: state
      entity_id: input_boolean.mailbox_guard_low_battery
      to: "on"
  condition: []
  action:
    - service: notify.notify
      data:
        message: Mailbox Guard Low Battery!
    - service: input_boolean.turn_off
      data:
        entity_id: input_boolean.mailbox_guard_low_battery
  mode: single
```

- Dashboard card

```yaml
type: entities
title: Mailbox Guard
show_header_toggle: false
entities:
  - type: conditional
    conditions: []
    row:
      type: sensor-entity
      entity: input_number.mailbox_guard_count
      name: Motion Count
  - type: button
    icon: mdi:sync
    name: Reset Count
    action_name: RESET
    tap_action:
      action: call-service
      service: input_number.set_value
      data:
        entity_id: input_number.mailbox_guard_count
        value: 0
  - type: conditional
    conditions: []
    row:
      type: sensor-entity
      entity: input_number.mailbox_guard_battery
      name: Battery
  - type: conditional
    conditions: []
    row:
      type: sensor-entity
      entity: input_number.mailbox_guard_rssi
      name: RSSI
  - type: conditional
    conditions: []
    row:
      type: sensor-entity
      entity: input_number.mailbox_guard_snr
      name: SNR
```

### MailBox Guard motion detection events

1. Your phone receives a notification from HA `You've Got Mail!`
2. The MailBox Guard will add a count to the `Motion Count`
3. When you fetch the mail select the `RESET` button

![](/img/HARestAPI_Notify.png)

![](/img/HARestAPI.png)
