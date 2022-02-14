#!/usr/bin/env python3


from google.auth.transport.requests import Request
from google.oauth2.credentials import Credentials
from google_auth_oauthlib.flow import InstalledAppFlow
from googleapiclient.discovery import build
import json5
import os


SCOPES = ['https://www.googleapis.com/auth/spreadsheets.readonly']

SPREADSHEET_ID = '1RL-Hre4N3SsjssjN0NvwV8UZm_Dx5jiuMXV5grPwMjQ'

creds = None
if os.path.exists('token.json'):
    creds = Credentials.from_authorized_user_file('token.json', SCOPES)
if not creds or not creds.valid:
    if creds and creds.expired and creds.refresh_token:
        creds.refresh(Request())
    else:
        flow = InstalledAppFlow.from_client_secrets_file('credentials.json', SCOPES)
        creds = flow.run_local_server(port = 0)
    with open('token.json', 'w') as token:
        token.write(creds.to_json())

service = build('sheets', 'v4', credentials=creds)
for group in ['Common', 'Menu', 'Race']:
    sheet = f'MKWii SP ({group}.bmg)'
    result = service.spreadsheets().values().get(
        spreadsheetId = SPREADSHEET_ID,
        range = sheet,
    ).execute()
    values = result.get('values', [])
    languages = {
        'E': 'English (PAL)',
        'F': 'French (PAL)',
        'G': 'German',
        'I': 'Italian',
        'J': 'Japanese',
        'K': 'Korean',
        'M': 'Spanish (NTSC)',
        'Q': 'French (NTSC)',
        'S': 'Spanish (PAL)',
        'N': 'Dutch',
    }
    for language in languages:
        if languages[language] not in values[0]:
            print(f'No column found for language {language}.', file=sys.stderr)
            continue
        index = values[0].index(languages[language])
        messages = {}
        for row in values[2:]:
            message_id = row[0]
            message = row[index]
            if message != '{{no|-}}':
                messages[message_id] = {
                    'font': 'regular',
                    'string': message.replace('\n', '\\n'),
                }
        data = json5.dumps(messages, ensure_ascii = False, indent = 4, quote_keys = True)
        path = os.path.join('assets', 'message', f'{group}SP_{language}.bmg.json5')
        file = open(path, 'w', encoding = 'utf-8')
        file.write(data)
