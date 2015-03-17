/* The group structure.	 */
struct group
  {
    char *gr_name;		/* Group name.	*/
    char *gr_passwd;		/* Password.	*/
    gid_t gr_gid;		/* Group ID.	*/
    char **gr_mem;		/* Member list.	*/
  };
/* Search for an entry with a matching group ID. */
 struct group *getgrgid (gid_t gid);

/* Search for an entry with a matching group name. */
 struct group *getgrnam (const char *name);


/* Rewind the group-file stream. */
  void setgrent (void);

/* Close the group-file stream. */
 void endgrent (void);

/* Read an entry from the group-file stream, opening it if necessary. */

 struct group *getgrent (void);
