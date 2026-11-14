# EXPOSX <img width="50" height="50" alt="image" src="https://github.com/user-attachments/assets/bc94c4ea-f656-4403-bf55-98ccbdf9bceb" />

EXPOSX, Common Vulnerability Exposure - Common Line Interace (C, NO AI).
We keep only the latest CVE (365d).

## compatibility
- exposx can run on Linux and MacOS.
- I implemented a docker_compose.yml if you need to use it on a Windows Machine or any other.
- The docker compose is only used to get the CLIENT running.
- The SERVER is not implemented in the docker compose, because it would be useless.
- (The server is making daily api call on the internet to update his CVE DATA,
meaning that hosting your own would not help you at all)
- You are still free to implement SERVER in your own DOCKER!

## roadmap
C Client Scanner -> C Server on VPS

C Client
At start it call the API to get the 3 last more critical CVE, and show them.
On search it will call the API and return a json to parse.

C Server
Everyday at 07:30 (PARIS-GMT) it call https://nvd.nist.gov/developers/vulnerabilities,
and retrieve all the new CVE.
On the other side it remove all cve older than 365 days from now.

Implementing a docker image in github package "registry"

## about the api that I clone
```
int yesterday
int today
RAW=curl "https://services.nvd.nist.gov/rest/json/cves/2.0?kevStartDate=2026-11-10T00:00:00.000Z&kevEndDate=2026-11-13T23:59:59.000Z"
FORMAT=curl "https://services.nvd.nist.gov/rest/json/cves/2.0?kevStartDate=yesterdayT00:00:00.000Z&kevEndDate=todayT23:59:59.000Z"
```
This would need to be parsed and pushed to SQLite.
When its done I need to TRY erasing the CVE older than 365 days.

## exposx usage
```
exposx -h -> usage/help
exposx -v -> verbose
exposx -s -> stdin

exposx$ search <(id | name)> or poc <id> or export <id> <(json | pretty)> or update or exit
```

## AI Policy
No AI is tolerated in this repo, this is all for fun and the love of the game.
It would be pretty sad dude!

<img width="88" height="31" alt="image" src="https://github.com/user-attachments/assets/9881f34c-e993-48e5-923f-e2b7a554257c" />

