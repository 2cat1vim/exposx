# exposx
CVE-CLI, Common Vulnerability Expose - Common Line Interace (C, NO AI).

# roadmap
C Client Scanner -> C Server on VPS

C Client
At start it call the API to get the 3 last more critical CVE, and show them.
On search it will call the API and return a json to parse.

C Server
Everyday at 07:30 (PARIS-GMT) it call https://nvd.nist.gov/developers/vulnerabilities,
and retrieve all the new CVE.
On the other side it remove all cve older than 365 days from now.

```
exposx -h -> usage/help
exposx -v -> verbose
exposx -s -> stdin

exposx$ search or update or exit
```

## AI Policy
No AI is tolerated in this repo!
Do not commit any vibecoded code :)
o not commit any vibecoded code :)
