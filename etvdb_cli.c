/* etvdb_cli - a simple command line frontend for thetvdb.com 
 * Copyright (C) 2013  Thomas Gstaedtner 
 *
 * This program is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation, either version 3 of the License, or 
 * (at your option) any later version. 
 *
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details. 
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 */

#define ERR(msg, args...) fprintf(stderr, "ERROR: "msg"\n", ## args)
 
#include <stdio.h>
#include <stdlib.h>
#include <Ecore.h>
#include <Ecore_Getopt.h>
#include <Eina.h>
#include <etvdb.h>

const Ecore_Getopt go_options = {
	"etvdb_cli",
	"%prog [options] <files>",
	"0.1",
	"(C) 2013  Thomas Gstaedtner",
	"This program is free software under the GNU GPL v3 or any later version.\n",
	"fetch data from TheTVDB.com (and rename files)\n",
	0,
	{
		ECORE_GETOPT_STORE_INT('e', "episode", "episode number"),
		ECORE_GETOPT_STORE_INT('s', "season", "season number"),
		ECORE_GETOPT_STORE_STR('E', "eid", "tvdb id of the episode"),
		ECORE_GETOPT_STORE_STR('N', "sid", "tvdb id of the series"),
		ECORE_GETOPT_STORE_STR('n', "name", "name, imdb id, or zap2it id of the series"),
		ECORE_GETOPT_STORE_STR('l', "lang", "set language for TVDB (default: en)"),
		ECORE_GETOPT_LICENSE('L', "license"),
		ECORE_GETOPT_COPYRIGHT('C', "copyright"),
		ECORE_GETOPT_VERSION('V', "version"),
		ECORE_GETOPT_HELP('h', "help"),
		ECORE_GETOPT_STORE_TRUE('H', "lang-help", "show available languages"),
		ECORE_GETOPT_SENTINEL
	}
};

Eina_Bool print_hash(const Eina_Hash *hash, const void *key, void *ser_data, void *fser_data)
{
	printf("  \'%s\': %s\n", (char *)key, (char *)ser_data);
	return EINA_TRUE;
}

void print_csv_episode(Episode *e)
{
	printf("%d|%d|%s|%s|", e->season, e->number, e->id, e->name);
	if (!e->overview)
		printf("|\n");
	else
		printf("%s|\n", e->overview);
}

void print_csv_head()
{
	printf("Season|Episode|ID|Name|Overview\n");
}

int main(int argc, char **argv)
{
	int go_index;
	int episode_num = 0, season_num = 0;
	int extra_args;
	char *episode_id = NULL, *series_id = NULL, *series_name = NULL;
	char *language = NULL;
	Eina_Bool go_quit = EINA_FALSE, lang_help = EINA_FALSE;
	Eina_List *series_list = NULL, *season_list = NULL, *l, *sl;
	Eina_Hash *languages = NULL;
	Episode *episode = NULL;
	Series *series = NULL;

	Ecore_Getopt_Value go_values[] = {
		ECORE_GETOPT_VALUE_INT(episode_num),
		ECORE_GETOPT_VALUE_INT(season_num),
		ECORE_GETOPT_VALUE_STR(episode_id),
		ECORE_GETOPT_VALUE_STR(series_id),
		ECORE_GETOPT_VALUE_STR(series_name),
		ECORE_GETOPT_VALUE_STR(language),
		ECORE_GETOPT_VALUE_BOOL(go_quit),
		ECORE_GETOPT_VALUE_BOOL(go_quit),
		ECORE_GETOPT_VALUE_BOOL(go_quit),
		ECORE_GETOPT_VALUE_BOOL(go_quit),
		ECORE_GETOPT_VALUE_BOOL(lang_help),
		ECORE_GETOPT_VALUE_NONE
	};

	if (!ecore_init()) {
		ERR("Ecore Init failed.");
		exit(EXIT_FAILURE);
	}

	if (!etvdb_init(NULL)) {
		ERR("etvdb Init failed.");
		exit(EXIT_FAILURE);
	}

	go_index = ecore_getopt_parse(&go_options, go_values, argc, argv);
	if (go_quit)
		exit(EXIT_SUCCESS);

	/* store if we have non-option arguments */
	extra_args = argc - go_index;

	/* language setup/help */
	if (language || lang_help) {
		languages = etvdb_languages_get(NULL);
		if (!languages) {
			ERR("Language List could not be generated.");
			exit(EXIT_FAILURE);
		}
	}

	if (lang_help) {
		printf("Supported Languages:\n");
		eina_hash_foreach(languages, print_hash, NULL);
		exit(EXIT_SUCCESS);
	}

	if (language) {
		if (!etvdb_language_set(languages, language)) {
			ERR("Language \'%s\' not supported.", language);
			exit(EXIT_FAILURE);
		}
	}

	/* a certain set of options is required to be useful, else we can quit right away */
	if ((episode_id || episode_num) && (extra_args) > 1) {
		ERR("You are looking for a Episode, but passed more than one file; please use only one file.");
		exit(EXIT_FAILURE);
	} else if (!series_id && !series_name && !episode_id) {
		ERR("You need to provide at least an Episode ID or an identifier for a Series.");
		exit(EXIT_FAILURE);
	} else if (episode_id && (episode_num || season_num || series_id || series_name)) {
		ERR("If you pass an Episode ID, no further search options are permitted, as they might conflict.");
		exit(EXIT_FAILURE);
	} else if (episode_num && !season_num) {
		ERR("If you're looking for a episode by number, you have to provide the season, too.");
		exit(EXIT_FAILURE);
	} else if (series_id && series_name) {
		ERR("You cannot use a Series ID and a Series name at the same time, they conflict.");
		exit(EXIT_FAILURE);
	}

	/* find the series - for now just pick the first one, might add interactive variant later. (TODO)*/
	if (series_name) {
		series_list = etvdb_series_find(series_name);
		if (!series_list) {
			ERR("Series \"%s\" not found.", series_name);
			exit(EXIT_FAILURE);
		} else {
			series = eina_list_nth(series_list, 0);
		}
	}

	/* make sure we have a valid series structure */
	if (!series)
		series = etvdb_series_by_id_get(series_id);

	/* initialize episode, if no episode requested, get all of them */
	if (episode_id)
		episode = etvdb_episode_by_id_get(episode_id);
	else if (episode_num && season_num)
		episode = etvdb_episode_by_number_get(series->id, season_num, episode_num);
	else
		etvdb_series_populate(series);


	/* if no files are passed, we just print everything requested in a simple CSV format */
	if (!extra_args) {
		print_csv_head();
		if (episode)
			print_csv_episode(episode);
		else if (!season_num) {
			EINA_LIST_FOREACH(series->seasons, l, season_list) {
				EINA_LIST_FOREACH(season_list, sl, episode)
					print_csv_episode(episode);
			}
		} else if (season_num) {
			season_list = eina_list_nth(series->seasons, season_num - 1);
			EINA_LIST_FOREACH(season_list, sl, episode)
				print_csv_episode(episode);
		}
	/* here we go into file mode */
	} else {
		// TODO
	}


	EINA_LIST_FREE(series_list, series)
		etvdb_series_free(series);

	etvdb_shutdown();
	ecore_shutdown();
	exit(EXIT_SUCCESS);
}
