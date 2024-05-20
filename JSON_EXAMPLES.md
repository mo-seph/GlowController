# General Control

Save current state:
`{"saveState":1}`
`{"loadState":1}`

# Behaviours

## Countdown Timer
Set time of countdown timer (if it's behaviour 6) to 23 seconds:
`{"update":6, "data":{"time":23}}`
Start countdown timer:
`{"update":6, "data":{"running":true}}`
Full Update:
`{"update":6, "data":{"time":30, "position":0.05, "length":0.9}}`