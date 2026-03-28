/*
 * ============================================================================
 *  Dragon Ball Z: Budokai — PS2 (BESLES-51233)
 *  Decompilazione Ghidra — REFACTORING & DOCUMENTAZIONE
 *  Architettura: MIPS R5900 (EE — Emotion Engine) PS2
 *
 *  File originale: SLES_512_33.c  (~5.4 MB, ~190.000 righe, 4.297 funzioni)
 *  Questo file documenta e rinomina le funzioni per modulo.
 *
 *  MODULI IDENTIFICATI
 *  -------------------
 *   [GS]   Graphics Synthesizer / VIF / GIF — rendering PS2
 *   [SYS]  Syscall EE Kernel (thread, semafori, DMA, cache)
 *   [DBG]  Debug / Deci2 / printf
 *   [MEM]  Memoria / heap / allocatore
 *   [CACHE] Cache ops (writeback, invalidate)
 *   [INTC] Interrupt controller
 *   [MATH] Utilità matematiche float
 *   [BTB]  Battle main (btb_main.c)
 *   [BTL]  Battle command IOP (btl_cmd.c)
 *   [BTF]  Battle field/player (btf_player.c)
 *   [AM]   Animation/Object/Texture (amMotion/amObject/amTexture.c)
 *   [SCR]  Scripting / TCB task (scr_func.c)
 *   [AURA] Proiettili / Aura / Kamehameha (aurashot.c)
 *   [MENU] Menu azioni/sprite (MenuActDraw.c)
 *   [OPT]  Opzioni di gioco (option.c)
 *   [SAT]  Modalità Satan/demo (satanmode.c)
 *   [ESS]  Sistema carte/capsule (ess_deck/list/shop/trade.c)
 * ============================================================================
 */

/* ============================================================
 *  TIPI BASE — invariati dall'output Ghidra (architettura MIPS)
 * ============================================================ */
typedef unsigned char    undefined;
typedef unsigned char    bool;
typedef unsigned char    byte;
typedef unsigned int     dword;
typedef unsigned int     uint;
typedef unsigned long    ulong;
typedef unsigned char    undefined1;
typedef unsigned short   undefined2;
typedef unsigned int     undefined4;
typedef unsigned long    undefined8;
typedef unsigned short   ushort;
typedef unsigned short   word;

/* ============================================================
 *  REGISTRI HARDWARE PS2 — indirizzi fisici mappati in memoria
 *  (nomi secondo il manuale ufficiale PS2 EE)
 * ============================================================ */
// GS — Graphics Synthesizer
#define REG_GS_PMODE     (*((volatile ulong*)0x12000000)) // Pixel Mode
#define REG_GS_SMODE2    (*((volatile ulong*)0x12000020)) // Sync Mode 2
#define REG_GS_DISPFB1   (*((volatile ulong*)0x12000070)) // Display Framebuffer 1
#define REG_GS_DISPLAY1  (*((volatile ulong*)0x12000080)) // Display 1
#define REG_GS_DISPFB2   (*((volatile ulong*)0x12000090)) // Display Framebuffer 2
#define REG_GS_DISPLAY2  (*((volatile ulong*)0x120000A0)) // Display 2
#define REG_GS_EXTDATA   (*((volatile ulong*)0x120000B0)) // External Data
#define REG_GS_BGCOLOR   (*((volatile ulong*)0x120000E0)) // Background Color
#define REG_GS_CSR       (*((volatile ulong*)0x12001000)) // Control/Status

// VIF1 — Vector Interface 1
#define REG_VIF1_STAT    (*((volatile uint*)0x10003C00)) // Status
#define REG_VIF1_FBRST   (*((volatile uint*)0x10003C10)) // Force Break Reset
#define REG_VIF1_ERR     (*((volatile uint*)0x10003C20)) // Error
#define REG_VIF1_FIFO    (*((volatile uint*)0x10005000)) // FIFO

// GIF — Graphics Interface
#define REG_GIF_CTRL     (*((volatile uint*)0x10003000)) // Control
#define REG_GIF_STAT     (*((volatile uint*)0x10003020)) // Status

// DMAC — DMA Controller canali VIF1 e GIF
#define REG_DMAC_1_VIF1_CHCR  (*((volatile uint*)0x10009000))
#define REG_DMAC_1_VIF1_MADR  (*((volatile uint*)0x10009010))
#define REG_DMAC_1_VIF1_QWC   (*((volatile uint*)0x10009020))
#define REG_DMAC_1_VIF1_TADR  (*((volatile uint*)0x10009030))
#define REG_DMAC_2_GIF_CHCR   (*((volatile uint*)0x1000A000))
#define REG_DMAC_2_GIF_MADR   (*((volatile uint*)0x1000A010))
#define REG_DMAC_2_GIF_QWC    (*((volatile uint*)0x1000A020))
#define REG_DMAC_2_GIF_TADR   (*((volatile uint*)0x1000A030))

// INTC — Interrupt Controller
#define REG_INTC_STAT    (*((volatile uint*)0x1000F000))

// RCNT — Root Counter
#define REG_RCNT3_MODE   (*((volatile uint*)0x10003810))

/*
 * Indirizzo base struttura configurazione GS (puntatore fisso nel BSS)
 * restituito da GS_GetConfigPtr(). Contiene: mode, width, height,
 * interlace, framebuffer_count, vsync_handler, vsync_id.
 */
#define GS_CONFIG_BASE   0x317110


/* ============================================================
 *  [MATH] — Utilità matematiche float
 * ============================================================ */

/*
 * UInt64ToFloat_Safe
 * ------------------
 * Converte un intero a 64-bit senza segno in float.
 * Ghidra non genera correttamente il cast diretto per ulong,
 * perciò il compilatore originale usa questo wrapper.
 * Se il valore è negativo (bit 63 = 1) lo dimezza, converte,
 * poi raddoppia per evitare overflow.
 *
 * Indirizzo originale: FUN_001000d0
 */
float UInt64ToFloat_Safe(ulong value)
{
    float result;
    if ((long)value < 1) {
        // Valore con bit alto impostato: dimezza → converti → raddoppia
        result = (float)__cvt_s64_to_f32(value >> 1 | value & 1);
        result = result + result;
    } else {
        result = (float)__cvt_s64_to_f32(value);
    }
    return result;
}


/*
 * String_IsGreaterThan
 * --------------------
 * Wrapper di strcmp: ritorna true se strA > strB (strcmp > 0).
 * Usato per ordinamento di stringhe (nomi personaggi, menu).
 *
 * Indirizzo originale: FUN_00100110
 */
bool String_IsGreaterThan(ulong strA, ulong strB)
{
    int result = strcmp_ps2(strA, strB);
    return 0 < result;
}


/* ============================================================
 *  [GS] — Graphics Synthesizer: configurazione display
 * ============================================================ */

/*
 * GS_GetConfigPtr
 * ---------------
 * Restituisce il puntatore alla struttura di configurazione del GS.
 * Indirizzo fisso: 0x317110 (BSS globale).
 * Layout struttura (offset in short/2 byte):
 *   [0]  mode         (0=NTSC, 1=PAL, 2=VESA, 5=PAL-50)
 *   [1]  video_mode   (2=576i, 3=480i, ...)
 *   [2]  interlaced   (0=progressivo, 1=interlacciato)
 *   [3]  field_id     (campo corrente dal GS_CSR)
 *   [4]  vsync_func   (puntatore handler vsync, 0=nessuno)
 *   [6]  framebuf_idx (0=context1, 1=context2)
 *   [8]  vsync_func2  (secondo handler)
 *   [0xc] vsync_id2
 *
 * Indirizzo originale: FUN_001002c0
 */
undefined4 GS_GetConfigPtr(void)
{
    return GS_CONFIG_BASE;
}


/*
 * GS_SetDisplayMode
 * -----------------
 * Imposta la modalità video del Graphics Synthesizer.
 * Configurabile per NTSC, PAL e modalità progressive.
 *
 * Parametri:
 *   mode      — 0=inizializza, 1=reset CSR, 5=aggiorna senza INTC
 *   width     — larghezza orizzontale (pixel)
 *   height    — altezza verticale (pixel)
 *   interlace — 0=progressivo, !=0=interlacciato
 *
 * Indirizzo originale: FUN_00100130
 */
void GS_SetDisplayMode(short mode, undefined2 width, undefined2 height, short interlace)
{
    undefined2 *config = (undefined2 *)GS_GetConfigPtr();

    if (mode == 1) {
        // Reset hard del GS tramite bit CSR
        REG_GS_CSR = 0x100;
    }
    else if (mode == 0) {
        // Inizializzazione completa: reset + configurazione + INTC
        REG_GS_CSR = 0x200;
        config[0] = width;
        // Legge il field ID corrente dal CSR
        ulong csr_val = REG_GS_CSR;
        config[1] = height;
        config[3] = (ushort)((ulong)csr_val >> 0x10) & 0xff;
        GsPutIMR();                      // Ripristina Interrupt Mask Register
        config[2] = (ushort)(interlace != 0);
        // Rimuove eventuale handler vsync precedente
        if (*(int *)(config + 4) != 0) {
            Intc_DisableVsync();
            RemoveIntcHandler();
            *(undefined4 *)(config + 6) = 0;
            *(undefined4 *)(config + 4) = 0;
        }
        SetGsCrt();  // syscall EE: imposta CRT
    }
    else if (mode == 5) {
        // Aggiornamento leggero: solo registri, senza toccare INTC
        ulong csr_val = REG_GS_CSR;
        config[2] = (ushort)(interlace != 0);
        config[0] = width;
        config[1] = height;
        config[3] = (ushort)((ulong)csr_val >> 0x10) & 0xff;
        SetGsCrt();
    }
}


/*
 * VIF1_Reset
 * ----------
 * Resetta il Vector Interface 1 e reinizializza il VU1
 * (Vector Unit 1 — usato per trasformazioni 3D e skinning).
 * Scrive nei registri FBRST ed ERR del VIF1, poi invia
 * il microprogram iniziale via FIFO.
 *
 * Indirizzo originale: FUN_001002d0
 */
void VIF1_Reset(void)
{
    REG_VIF1_FBRST = 1;   // Force Break: ferma VIF1
    REG_VIF1_ERR   = 2;   // Azzera errori
    SYNC(0);
    // Abilita il bit ERE nel COP2 (VU1 control)
    ulong vu1_ctrl = _cfc2(COP2_CTRL);
    _ctc2((uint)vu1_ctrl | 0x200);
    SYNC(0x10);
    // Carica il microprogram VU1 dalla struttura GS config (offset 0x10)
    REG_VIF1_FIFO = (int)uRam00317120;
    // ... (blocco di 8 quadword al VIF1 FIFO)
    REG_GIF_CTRL  = 1;    // Reset GIF
}


/*
 * GS_BuildDisplayPacket
 * ----------------------
 * Costruisce un pacchetto DMA per i registri GS_DISPLAY e GS_SMODE2.
 * Calcola i valori dei campi MAGH, MAGV in base a risoluzione
 * e modalità (NTSC=2, PAL=3) letti dalla struttura config.
 *
 * Parametri:
 *   packet   — buffer destinazione per il pacchetto GS
 *   psm      — formato pixel (4 bit)
 *   width    — larghezza framebuffer in pixel
 *   height   — altezza in pixel
 *   pos_x    — offset X schermo
 *   pos_y    — offset Y schermo
 *
 * Indirizzo originale: FUN_00100338
 */
void GS_BuildDisplayPacket(undefined8 *packet, short psm, short width,
                           short height, short pos_x, short pos_y)
{
    ushort *config = (ushort *)GS_GetConfigPtr();
    packet[0] = 0x66;  // GIF TAG: PACKED mode, NLOOP=0

    ushort video_mode = config[0];
    undefined8 context;
    if (video_mode == 0)       context = 2;
    else if (config[2] == 0)   context = 1;
    else                       context = 3;

    packet[1] = context;
    // Campo DISPLAY: FBP, PSM, DBW
    packet[2] = ((long)(int)psm & 0xfU) << 0xf |
                ((long)(((int)width + 0x3f) >> 6) & 0x3fU) << 9;

    ushort video_type = (ushort)config[1];
    int mag_h, mag_v = (int)pos_y;

    if (video_type == 2) {
        // NTSC 480i
        if (video_mode != 1) {
            mag_h = (width + 0x9ff) / width;
            mag_v += 0x19;
        } else {
            mag_h = (width + 0x9ff) / width;
        }
        // calcola DY per offset verticale
        ulong dy = (long)((int)pos_x * mag_h) + 0x27c;
        packet[3] = (long)(mag_h - 1) << 0x17 |
                    (long)(mag_h * width - 1) << 0x20 |
                    dy & 0xfff |
                    (long)(height - 1) << 0x2c |
                    ((long)mag_v & 0xfffU) << 0xc;
    } else if (video_type == 3) {
        // PAL 576i
        if (video_mode != 1) {
            mag_h = (width + 0x9ff) / width;
            mag_v += 0x24;
        } else {
            mag_h = (width + 0x9ff) / width;
        }
        ulong dy = (long)((int)pos_x * mag_h) + 0x290;
        packet[3] = (long)(mag_h - 1) << 0x17 |
                    (long)(mag_h * width - 1) << 0x20 |
                    dy & 0xfff |
                    (long)(height - 1) << 0x2c |
                    ((long)mag_v & 0xfffU) << 0xc;
    } else {
        // Modalità non supportata: errore
        Debug_Printf(0x353300, video_type, (long)(int)config, (ulong)video_mode);
    }
    packet[4] = 0;
}


/*
 * GS_ApplyDisplayRegisters
 * ------------------------
 * Applica i valori del pacchetto display ai registri hardware GS.
 * Usa context1 (framebuffer singolo) o context2 (doppio buffer)
 * in base all'indice letto dalla config.
 *
 * Indirizzo originale: FUN_001005a8
 */
void GS_ApplyDisplayRegisters(undefined8 *packet)
{
    int config = GS_GetConfigPtr();
    if (*(short *)(config + 6) == 1) {
        // Context 1 — framebuffer singolo
        REG_GS_PMODE    = packet[0];
        REG_GS_DISPFB1  = packet[2];
        REG_GS_DISPLAY1 = packet[3];
        REG_GS_EXTDATA  = packet[4];
    } else {
        // Context 2 — doppio buffer (default)
        REG_GS_PMODE    = packet[0];
        REG_GS_SMODE2   = packet[1];
        REG_GS_DISPFB2  = packet[2];
        REG_GS_DISPLAY2 = packet[3];
        REG_GS_BGCOLOR  = packet[4];
    }
}


/*
 * GS_CalcTextureBufferWidth
 * -------------------------
 * Calcola il DBW (Display Buffer Width) in unità da 64 pixel
 * dato il formato pixel e le dimensioni della texture.
 * Il risultato è usato nei registri BITBLTBUF/TRXREG.
 *
 * Indirizzo originale: FUN_00100668
 */
int GS_CalcTextureBufferWidth(uint psm, short width, short height)
{
    ulong *config = (ulong *)GS_GetConfigPtr();
    int aligned_w, aligned_h;

    aligned_w = (width + 0x3f) >> 6;  // ceil(width / 64)

    if ((psm & 2) == 0) {
        // 32bpp / 24bpp
        aligned_h = (height + 0x1f) >> 5;
    } else {
        // 16bpp / 8bpp / 4bpp
        aligned_h = (height + 0x3f) >> 6;
    }

    int result = aligned_w * aligned_h;

    // Doppio se modalità interlacciata
    if ((*config & 0xffff0000ffff) == 1) {
        result = result * 0x10000;
    } else {
        result = result * 0x20000;
    }
    return result >> 0x10;
}


/*
 * GS_SetupTexturePacket_Context1
 * ------------------------------
 * Prepara il pacchetto GIF per upload texture nel contesto 1.
 * Scrive i registri TEX0_1, TEX1_1, CLAMP_1, MIPTBP1_1.
 * Usato per il layer principale (personaggi, stage).
 *
 * Indirizzo originale: GSRender_SetupTexturePacket (già rinominato da Ghidra)
 */
/* [già documentato nel file Ghidra come GSRender_SetupTexturePacket] */


/*
 * GIF_DmaSend
 * -----------
 * Invia un pacchetto al GIF tramite DMA channel 2.
 * Attende che il canale sia libero (polling su CHCR bit 8),
 * poi avvia il trasferimento. Timeout dopo ~16M cicli.
 *
 * Parametri:
 *   packet_addr — indirizzo quadword-allineato del pacchetto GIF
 *
 * Return: 0 = successo, -1 = timeout DMA
 *
 * Indirizzo originale: FUN_00100a20
 */
undefined4 GIF_DmaSend(ulong packet_addr)
{
    uint chcr = REG_DMAC_2_GIF_CHCR;
    uint timeout = 0;

    while (true) {
        if ((chcr & 0x100) == 0) {
            // DMA libero: imposta indirizzo, dimensione e avvia
            ulong qwc = (uint)*(undefined8 *)packet_addr & 0x7fff;
            REG_DMAC_2_GIF_QWC  = (uint)(qwc + 1);
            uint madr = (uint)packet_addr & 0xfffffff;
            if ((packet_addr & 0x70000000) == 0x70000000)
                madr |= 0x80000000;  // bit SPR (Scratchpad RAM)
            REG_DMAC_2_GIF_MADR = madr;
            REG_DMAC_2_GIF_CHCR = 0x101;  // STR=1, DIR=1 (from memory)
            return 0;
        }
        if (0x1000000 < timeout++) break;
        chcr = REG_DMAC_2_GIF_CHCR;
    }
    // Timeout: log errore e dump registri DMA
    Debug_Printf(0x353338, 0x1000000, packet_addr);
    return 0xffffffff;
}


/*
 * GS_WaitVSync
 * ------------
 * Attende il segnale VSync del GS (vertical blank).
 * Usa l'interrupt INTC_STAT bit 2 (GS interrupt).
 * Se è attivo un handler vsync custom, lo esegue.
 *
 * Return: 1 se campo dispari (odd field), 0 se pari
 *
 * Indirizzo originale: FUN_00100b08
 */
uint GS_WaitVSync(void)
{
    short *config = (short *)GS_GetConfigPtr();
    uint field;

    if (*(int *)(config + 4) == 0) {
        // Nessun handler custom: polling diretto su INTC
        Vsync_PollInterrupt();
        field = 1;
        if (*config == 1) {
            field = (uint)(REG_GS_CSR >> 0xd) & 1;
        }
    } else {
        // Handler custom (SetVSyncFlag)
        ulong csr = Vsync_WaitWithFlag();
        field = 1;
        if (*config == 1) {
            field = (uint)(csr >> 0xd) & 1;
        }
    }
    return field;
}


/*
 * VIF1_GIF_WaitIdle
 * -----------------
 * Attende che la pipeline VIF1 + GIF sia completamente vuota.
 * Controlla in sequenza: DMA VIF1, DMA GIF, VIF1 STAT, COP2, GIF STAT.
 * Se param_1=0: attesa bloccante con timeout e log degli errori.
 * Se param_1!=0: polling immediato, ritorna bitmask degli stadi occupati.
 *
 * Indirizzo originale: FUN_00100ba0
 */
uint VIF1_GIF_WaitIdle(long blocking)
{
    if (blocking == 0) {
        // Modalità bloccante: attende ogni stadio in sequenza
        ulong timeout = 0;
        uint status;

        // 1. DMA VIF1
        status = REG_DMAC_1_VIF1_CHCR;
        while ((status & 0x100) != 0) {
            if (0x1000000 < timeout++) { Debug_DumpGSState(); return 0xffffffff; }
            status = REG_DMAC_1_VIF1_CHCR;
        }
        // 2. DMA GIF
        status = REG_DMAC_2_GIF_CHCR;
        while ((status & 0x100) != 0) {
            if (0x1000000 < timeout++) { Debug_DumpGSState(); return 0xffffffff; }
            status = REG_DMAC_2_GIF_CHCR;
        }
        // 3. VIF1 pipeline (bit 0-1: attesa micro, bit 24-28: stall)
        status = REG_VIF1_STAT;
        if ((status & 0x1f000003) != 0) {
            do {
                if (0x1000000 < timeout++) { Debug_DumpGSState(); return 0xffffffff; }
                status = REG_VIF1_STAT;
            } while ((status & 0x1f000003) != 0);
        }
        // 4. COP2 (VU1): bit 8 = E-bit busy
        ulong cop2 = _cfc2(COP2_CTRL);
        while ((cop2 & 0x100) != 0) {
            if (0x1000000 < timeout++) { Debug_DumpGSState(); return 0xffffffff; }
            cop2 = _cfc2(COP2_CTRL);
        }
        // 5. GIF: bit 10-11 = PATH in uso
        status = REG_GIF_STAT;
        while ((status & 0xc00) != 0) {
            if (0x1000000 < timeout++) { Debug_DumpGSState(); return 0xffffffff; }
            status = REG_GIF_STAT;
        }
        return 0;
    } else {
        // Modalità non bloccante: ritorna bitmask stadi occupati
        uint busy = 0;
        if ((REG_DMAC_1_VIF1_CHCR & 0x100) != 0) busy |= 1;
        if ((REG_DMAC_2_GIF_CHCR  & 0x100) != 0) busy |= 2;
        if ((REG_VIF1_STAT & 0x1f000003) != 0)    busy |= 4;
        if ((_cfc2(COP2_CTRL) & 0x100) != 0)      busy |= 8;
        if ((REG_GIF_STAT & 0xc00) != 0)           busy |= 0x10;
        return busy;
    }
}


/*
 * GS_SetVsyncCallback
 * -------------------
 * Registra (o rimuove) un handler per l'interrupt VSync.
 * Se handler=0, rimuove il vecchio handler.
 * Se handler!=0, sostituisce quello precedente.
 *
 * Return: puntatore al vecchio handler
 *
 * Indirizzo originale: FUN_00100eb8
 */
int GS_SetVsyncCallback(long handler_func)
{
    int config_base = GS_GetConfigPtr();
    int old_handler = *(int *)(config_base + 8);

    if (handler_func == 0) {
        // Rimozione handler
        Intc_DisableVsync();
        RemoveIntcHandler();
        *(undefined4 *)(config_base + 8)  = 0;
        *(undefined4 *)(config_base + 0xc) = 0;
    } else {
        if (old_handler != 0) {
            // Sostituisce handler esistente
            Intc_DisableVsync();
            RemoveIntcHandler();
        }
        *(int *)(config_base + 8) = (int)handler_func;
        undefined4 handler_id = AddIntcHandler2();
        *(undefined4 *)(config_base + 0xc) = handler_id;
        Intc_EnableVsync();
    }
    return old_handler;
}


/*
 * GS_CalcFramebufferOffset_Context1
 * ----------------------------------
 * Calcola l'offset DISPLAY (X, Y) per il contesto 1.
 * I valori dipendono dalla risoluzione del framebuffer corrente.
 *
 * Indirizzo originale: FUN_00100f58
 */
void GS_CalcFramebufferOffset_Context1(int display_reg_base, short cx, short cy, short interlaced)
{
    long fb_width  = (ushort)((ulong)*(undefined8 *)(display_reg_base + 0x30) >> 0x10) & 0x7ff;
    long fb_height = (ushort)((ulong)*(undefined8 *)(display_reg_base + 0x30) >> 0x30) & 0x7ff;
    long off_y = (long)cy - ((ulong)(long)(int)(fb_height + 1) >> 1);
    ulong dy;
    if (interlaced == 0)
        dy = off_y << 0x24;
    else
        dy = off_y * 0x10 + 8 << 0x20;

    *(ulong *)(display_reg_base + 0x20) =
        ((long)cx - ((ulong)(long)(int)(fb_width + 1) >> 1)) * 0x10 | dy;
}


/* ============================================================
 *  [GS] Texture full state
 * ============================================================ */

/*
 * GS_InitFramebufferTextureState
 * --------------------------------
 * Inizializza entrambi i context GS con la configurazione texture
 * completa: TEX0, TEX1, CLAMP, MIPTBP, SCISSOR, TEST, ALPHA.
 * Chiamata all'avvio del renderer e dopo ogni cambio risoluzione.
 *
 * Indirizzo originale: GSRender_InitTextureFullState (già rinominato da Ghidra)
 */


/* ============================================================
 *  [GS] Flip buffer / present frame
 * ============================================================ */

/*
 * GS_PresentFrame
 * ---------------
 * Scambia i framebuffer e avvia il trasferimento GIF del frame corrente.
 * Alterna tra i due packet display (offset 0x0 e 0x28 nella struttura).
 *
 * Indirizzo originale: FUN_00101530
 */
undefined4 GS_PresentFrame(int display_struct_base, uint frame_index)
{
    // Applica il packet relativo al buffer corrente (0 o 1)
    GS_ApplyDisplayRegisters((undefined8 *)(display_struct_base +
                              (frame_index & 1) * 0x28));
    // Avvia DMA GIF dell'altro buffer
    if ((frame_index & 1) == 0)
        return GIF_DmaSend((long)(display_struct_base + 0x50));
    else
        return GIF_DmaSend((long)(display_struct_base + 0x1c0));
}


/* ============================================================
 *  [SYS] EE Kernel — Syscall wrappers
 *  Tutte le funzioni seguenti sono thin wrapper attorno alle
 *  syscall del kernel EE PlayStation 2.
 *  I numeri corrispondono al manuale ufficiale EE Core Software.
 * ============================================================ */

/* syscall(2)  */ void SetGsCrt(void)            { syscall(2);    }
/* syscall(4)  */ void Exit(void)                { syscall(4);    }
/* syscall(6)  */ void LoadExecPS2(void)         { syscall(6);    }
/* syscall(7)  */ void ExecPS2(void)             { syscall(7);    }
/* syscall(0xd)*/ void SetVTLBRefillHandler(void){ syscall(0xd);  }
/* syscall(0xe)*/ void SetVCommonHandler(void)   { syscall(0xe);  }
/* syscall(0x10)*/ void AddIntcHandler2(void)    { syscall(0x10); }
/* syscall(0x11)*/ void RemoveIntcHandler(void)  { syscall(0x11); }
/* syscall(0x12)*/ void AddDmacHandler(void)     { syscall(0x12); }
/* syscall(0x13)*/ void RemoveDmacHandler(void)  { syscall(0x13); }
/* syscall(0x14)*/ void _EnableIntc(void)        { syscall(0x14); }
/* syscall(0x15)*/ void _DisableIntc(void)       { syscall(0x15); }
/* syscall(0x16)*/ void _EnableDmac(void)        { syscall(0x16); }
/* syscall(0x17)*/ void _DisableDmac(void)       { syscall(0x17); }
/* syscall(0x20)*/ void CreateThread(void)       { syscall(0x20); }
/* syscall(0x22)*/ void StartThread(void)        { syscall(0x22); }
/* syscall(0x24)*/ void ExitDeleteThread(void)   { syscall(0x24); }
/* syscall(0x29)*/ void ChangeThreadPriority(void){ syscall(0x29);}
/* syscall(0x2b)*/ void RotateThreadReadyQueue(void){ syscall(0x2b); }
/* syscall(0x2f)*/ void GetThreadId(void)        { syscall(0x2f); }
/* syscall(0x30)*/ void ReferThreadStatus(void)  { syscall(0x30); }
/* syscall(0x32)*/ void SleepThread(void)        { syscall(0x32); }
/* syscall(0x33)*/ void WakeupThread(void)       { syscall(0x33); }
/* syscall(0x35)*/ void CancelWakeupThread(void) { syscall(0x35); }
/* syscall(0x37)*/ void SuspendThread(void)      { syscall(0x37); }
/* syscall(0x39)*/ void ResumeThread(void)       { syscall(0x39); }
/* syscall(0x3e)*/ void EndOfHeap(void)          { syscall(0x3e); }
/* syscall(0x40)*/ void CreateSema(void)         { syscall(0x40); }
/* syscall(0x41)*/ void DeleteSema(void)         { syscall(0x41); }
/* syscall(0x42)*/ void SignalSema(void)         { syscall(0x42); }
/* syscall(0x44)*/ void WaitSema(void)           { syscall(0x44); }
/* syscall(0x45)*/ void PollSema(void)           { syscall(0x45); }
/* syscall(0x4a)*/ void SetOsdConfigParam(void)  { syscall(0x4a); }
/* syscall(0x4b)*/ void GetOsdConfigParam(void)  { syscall(0x4b); }
/* syscall(100) */ void FlushCache(void)         { syscall(100);  }
/* syscall(0x71)*/ void GsPutIMR(void)           { syscall(0x71); }
/* syscall(0x73)*/ void SetVSyncFlag(void)        { syscall(0x73); }
/* syscall(0x74)*/ void SetSyscall(void)         { syscall(0x74); }
/* syscall(0x76)*/ void sceSifDmaStat(void)      { syscall(0x76); }
/* syscall(0x77)*/ void sceSifSetDma(void)       { syscall(0x77); }
/* syscall(0x78)*/ void sceSifSetDChain(void)    { syscall(0x78); }
/* syscall(0x79)*/ void sceSifSetReg(void)       { syscall(0x79); }
/* syscall(0x7a)*/ void sceSifGetReg(void)       { syscall(0x7a); }
/* syscall(0x7b)*/ void ExecOSD(void)            { syscall(0x7b); }
/* syscall(0x7c)*/ void Deci2Call(void)          { syscall(0x7c); }
/* syscall(0x7f)*/ void GetMemorySize(void)      { syscall(0x7f); }
/* syscall(0x82)*/ void InitTLB(void)            { syscall(0x82); }
/* syscall(0xfc)*/ void SetAlarm(void)           { syscall(0xfc); }

// Varianti interrupt-safe (chiamabili da handler)
/* syscall(-26) */ void _iWakeupThread(void)     { syscall(0xffffffffffffffcc); }
/* syscall(-24) */ void _iSuspendThread(void)    { syscall(0xffffffffffffffc8); }
/* syscall(-67) */ void iSignalSema(void)        { syscall(0xffffffffffffffbd); }
/* syscall(-103)*/ void iFlushCache(void)        { syscall(0xffffffffffffff98); }
/* syscall(-104)*/ void _iEnableDmac(void)       { syscall(0xffffffffffffffe4); }
/* syscall(-27) */ void iReferThreadStatus(void) { syscall(0xffffffffffffffcf); }
/* syscall(-119)*/ void isceSifSetDma(void)      { syscall(0xffffffffffffff89); }
/* syscall(-120)*/ void isceSifSetDChain(void)   { syscall(0xffffffffffffff88); }
/* syscall(-26) */ void _iEnableIntc(void)       { syscall(0xffffffffffffffe6); }
/* syscall(-27) */ void _iDisableIntc(void)      { syscall(0xffffffffffffffe5); }
/* syscall(-28) */ void _iDisableDmac(void)      { syscall(0xffffffffffffffe3); }


/* ============================================================
 *  [INTC] — Interrupt controller wrappers
 * ============================================================ */

/*
 * Intc_DisableVsync
 * -----------------
 * Disabilita l'interrupt VSync (INTC_STAT bit 2 = GS).
 * Gestisce correttamente la sezione critica (ERL check).
 *
 * Indirizzo originale: FUN_001024c0
 */
undefined8 Intc_DisableVsync(void)
{
    uint in_kernel = Status & 0x10000;
    if (in_kernel) DI_safe();
    undefined8 ret = _DisableIntc();
    SYNC(0);
    if (in_kernel) EI_safe();
    return ret;
}

/*
 * Intc_EnableVsync
 * ----------------
 * Abilita l'interrupt VSync.
 * Indirizzo originale: FUN_00102528
 */
undefined8 Intc_EnableVsync(void)
{
    uint in_kernel = Status & 0x10000;
    if (in_kernel) DI_safe();
    undefined8 ret = _EnableIntc();
    SYNC(0);
    if (in_kernel) EI_safe();
    return ret;
}

/*
 * Dmac_Disable / Dmac_Enable
 * Indirizzo originale: FUN_00102590 / FUN_001025f8
 */
undefined8 Dmac_Disable(void)
{
    uint in_kernel = Status & 0x10000;
    if (in_kernel) DI_safe();
    undefined8 ret = _DisableDmac();
    SYNC(0);
    if (in_kernel) EI_safe();
    return ret;
}

undefined8 Dmac_Enable(void)
{
    uint in_kernel = Status & 0x10000;
    if (in_kernel) DI_safe();
    undefined8 ret = _EnableDmac();
    SYNC(0);
    if (in_kernel) EI_safe();
    return ret;
}

/* iEnableIntc / iDisableIntc / iEnableDmac / iDisableDmac
   versioni interrupt-safe (FUN_00102660/80/a0/c0) */
void iIntc_Enable(void)  { _iEnableIntc();  SYNC(0); }
void iIntc_Disable(void) { _iDisableIntc(); SYNC(0); }
void iDmac_Enable(void)  { _iEnableDmac();  SYNC(0); }
void iDmac_Disable(void) { _iDisableDmac(); SYNC(0); }


/* ============================================================
 *  [SYS] Vsync polling (usato quando non c'è handler INTC)
 * ============================================================ */

/*
 * Vsync_ClearFlag
 * ---------------
 * Azzera il flag VSync interno (usato dal kernel libgraph).
 * Indirizzo originale: FUN_00101e60
 */
void Vsync_ClearFlag(void)
{
    // Azzerano i 4 byte interni al buffer libkernl (offset 0x10)
    s_PsIIlibkernl2430_00317140[0x10] = '\0';
    s_PsIIlibkernl2430_00317140[0x11] = '\0';
    s_PsIIlibkernl2430_00317140[0x12] = '\0';
    s_PsIIlibkernl2430_00317140[0x13] = '\0';
}

/*
 * Vsync_PollInterrupt
 * --------------------
 * Attende il VSync tramite polling su INTC_STAT bit 2.
 * Ritorna true se è stato elaborato un frame pari.
 *
 * Indirizzo originale: FUN_00101e70
 */
bool Vsync_PollInterrupt(void)
{
    bool was_enabled = DI_safe();
    REG_INTC_STAT = 4;   // Cancella bit GS interrupt
    SYNC(0);
    if (was_enabled) EI_safe();

    uint stat;
    do { stat = REG_INTC_STAT; } while ((stat & 4) == 0);

    was_enabled = DI_safe();
    REG_INTC_STAT = 4;
    SYNC(0);
    if (!was_enabled) return false;
    return EI_safe();
}

/*
 * Vsync_WaitWithFlag
 * ------------------
 * Versione che usa SetVSyncFlag (syscall 0x73) prima del polling.
 * Ritorna il valore del GS_CSR al momento del VSync.
 *
 * Indirizzo originale: FUN_00101f00
 */
undefined8 Vsync_WaitWithFlag(void)
{
    SetVSyncFlag();
    bool was_enabled = DI_safe();
    REG_INTC_STAT = 4;
    SYNC(0);
    if (was_enabled) EI_safe();
    uint stat;
    do { stat = REG_INTC_STAT; } while ((stat & 4) == 0);
    was_enabled = DI_safe();
    REG_INTC_STAT = 4;
    SYNC(0);
    if (was_enabled) EI_safe();
    // uStack_18 = valore GS_CSR salvato dall'handler
    return uStack_18;
}


/* ============================================================
 *  [MEM] — Heap / allocatore
 * ============================================================ */

/*
 * Heap_Alloc
 * ----------
 * Allocatore heap semplice per l'EE: alloca 'size' byte
 * avanzando il puntatore uRam00317154 (heap pointer).
 * Non gestisce la deallocation (bump allocator).
 *
 * Parametri:
 *   size — byte da allocare
 *
 * Return: indirizzo del blocco, 0xFFFFFFFF se esaurito
 *
 * Indirizzo originale: FUN_001020e0
 */
uint Heap_Alloc(int size)
{
    uint in_kernel = Status & 0x10000;
    while ((Status & 0x10000) != 0) { DI(); SYNC(0x10); }

    uint new_top = uRam00317154 + size;
    uint heap_end = EndOfHeap();

    if (heap_end < new_top) {
        // Heap esaurito: imposta errno = 0xC (ENOMEM)
        undefined4 *errno_ptr = (undefined4 *)Kernel_GetErrnoPtr();
        *errno_ptr = 0xc;
        if (in_kernel) EI();
        uRam00317154 = uRam00317154;   // nessun avanzamento
        return 0xffffffff;
    }

    uint allocated = uRam00317154;
    if (in_kernel) EI();
    uRam00317154 = new_top;
    return allocated;
}


/* ============================================================
 *  [CACHE] — Cache operations
 * ============================================================ */

/*
 * Cache_WritebackRange
 * --------------------
 * Esegue writeback (flush) delle linee di cache D$ nel range [start, end].
 * Usa l'istruzione CACHE 0x14 (Index Writeback Invalidate).
 * L'offset è allineato a 64 byte (linea cache PS2).
 *
 * Indirizzo originale: FUN_00102230
 */
void Cache_WritebackRange(uint start, uint end)
{
    int offset = 0;
    do {
        SYNC(0);
        cacheOp(0x10, offset);    // Index Load Tag
        SYNC(0);
        uint tag = (TagLo & 0xfffff000) + offset;
        if (start <= tag && tag <= end) {
            SYNC(0);
            cacheOp(0x14, offset);  // Index Writeback Invalidate
            SYNC(0);
        }
        // Seconda way (offset+1)
        SYNC(0);
        cacheOp(0x10, offset + 1);
        SYNC(0);
        tag = (TagLo & 0xfffff000) + offset;
        if (start <= tag && tag <= end) {
            SYNC(0);
            cacheOp(0x14, offset + 1);
            SYNC(0);
        }
        SYNC(0);
        offset += 0x40;
    } while (offset < 0x1000);
}

/*
 * Cache_WritebackRange_Safe
 * -------------------------
 * Wrapper thread-safe di Cache_WritebackRange.
 * Disabilita gli interrupt prima, li riabilita dopo.
 *
 * Indirizzo originale: FUN_001022d8
 */
bool Cache_WritebackRange_Safe(uint start, uint end)
{
    uint in_kernel = Status & 0x10000;
    if (in_kernel) DI_safe();
    Cache_WritebackRange(start & 0xffffffc0, end & 0xffffffc0);
    if (in_kernel) return EI_safe();
    return false;
}

/*
 * Cache_InvalidateRange
 * ---------------------
 * Invalida (senza writeback) le linee di D$ nel range [start, end].
 * Usa CACHE 0x16 (Index Invalidate).
 * Usata dopo il DMA: la CPU deve rileggere i dati aggiornati.
 *
 * Indirizzo originale: FUN_00102370
 */
void Cache_InvalidateRange(uint start, uint end)
{
    int offset = 0;
    do {
        SYNC(0);
        cacheOp(0x10, offset);
        SYNC(0);
        uint tag = (TagLo & 0xfffff000) + offset;
        if (start <= tag && tag <= end) {
            SYNC(0);
            cacheOp(0x16, offset);   // Index Invalidate
            SYNC(0);
        }
        SYNC(0);
        cacheOp(0x10, offset + 1);
        SYNC(0);
        tag = (TagLo & 0xfffff000) + offset;
        if (start <= tag && tag <= end) {
            SYNC(0);
            cacheOp(0x16, offset + 1);
            SYNC(0);
        }
        SYNC(0);
        offset += 0x40;
    } while (offset < 0x1000);
}


/* ============================================================
 *  [SCR] — Task Control Block (TCB) — sistema multitasking
 * ============================================================ */

/*
 * TCB_Init
 * --------
 * Inizializza il sistema TCB (Task Control Block) usato da scr_func.c
 * come scheduler cooperativo leggero sopra i thread EE.
 * Crea una lista bidirezionale con sentinel Head e Tail,
 * poi inizializza 256 slot task in un pool lineare.
 *
 * Struttura TCB (offset):
 *   +0x00  flags/stato  (0=libero, 1=running, 2=waiting)
 *   +0x04  priorità
 *   +0x08  callback
 *   +0x44  nome (stringa 20 byte)
 *   +0x80  dimensione slot
 *
 * Indirizzo originale: FUN_001534d0
 */
void TCB_Init(void)
{
    uGpffff8900 = 0;
    uGpffff8904 = 0;
    uGpffff8908 = 0;

    // Inizializza sentinel Head
    strncpy_ps2(0x4ecc30, (undefined1 (*)[16])s_TCB_Head_003585a0, 0x14);
    uRam004ecca4 = 0;
    uRam004ecc54 = 0;
    uRam004ecc58 = 0x4eccb0;  // puntatore a Tail
    uRam004ecc44 = 0;
    uRam004ecc48 = 1;
    uRam004ecc5c = 0xd020a0c;
    uRam004ecca0 = 0xd020a0c;
    uRam004ecc4c = 0;
    uRam004ecc50 = 0;

    // Inizializza sentinel Tail
    strncpy_ps2(0x4eccb0, (undefined1 (*)[16])s_TCB_Tail_003585b0, 0x14);
    uRam004ecd24 = 0xffffffff;
    uRam004eccd4 = 0x4ecc30;  // puntatore a Head
    uRam004eccd8 = 0;
    uRam004eccc4 = 0;
    uRam004eccc8 = 1;
    uRam004eccdc = 0xd020a0c;
    uRam004ecd20 = 0xd020a0c;
    uRam004ecccc = 0;
    uRam004eccd0 = 0;

    // Popola 256 slot (0x80 byte ciascuno) a partire da 0x4ecd30
    for (int i = 0; i < 0x100; i++) {
        *(int *)(i * 4 + 0x4f4d30) = i * 0x80 + 0x4ecd30;
        *(undefined4 *)(i * 0x80 + 0x4ecd5c) = 0xd020a0c;  // magic idle
        *(undefined4 *)(i * 0x80 + 0x4ecda0) = 0xd020a0c;
    }
}


/* ============================================================
 *  [DBG] — Debug / Deci2 (comunicazione con host PS2 debugger)
 * ============================================================ */

/*
 * Le funzioni FUN_00102b80 … FUN_00102d38 sono tutte wrapper
 * sottili attorno a Deci2Call() con parametri diversi.
 * Deci2Call è il protocollo di debug PS2 (seriale via IOP).
 *
 * Rinomina:
 *   FUN_00102b80 → Deci2_Open
 *   FUN_00102bc8 → Deci2_Close
 *   FUN_00102bf0 → Deci2_Send
 *   FUN_00102c20 → Deci2_Recv
 *   FUN_00102c48 → Deci2_SendAsync
 *   FUN_00102c80 → Deci2_RecvAsync
 *   FUN_00102ce8 → Deci2_Poll
 *   FUN_00102d10 → Deci2_Flush
 *   FUN_00102d38 → Deci2_GetStatus
 */
void Deci2_Open(void)      { Deci2Call(); }
void Deci2_Close(void)     { Deci2Call(); }
void Deci2_Send(void)      { Deci2Call(); }
void Deci2_Recv(void)      { Deci2Call(); }
void Deci2_SendAsync(void) { Deci2Call(); }
void Deci2_RecvAsync(void) { Deci2Call(); }
void Deci2_Poll(void)      { Deci2Call(); }
void Deci2_Flush(void)     { Deci2Call(); }
void Deci2_GetStatus(void) { Deci2Call(); }

/*
 * Debug_Printf
 * ------------
 * Printf di debug inviata via Deci2 all'host.
 * Indirizzo originale: FUN_00119f78 (chiamata ovunque con string literals)
 */
// extern void Debug_Printf(long fmt_addr, ...);


/* ============================================================
 *  [SYS] Ring buffer / coda FIFO per eventi
 * ============================================================ */

/*
 * RingBuffer_Init
 * ---------------
 * Inizializza una struttura coda FIFO circolare con testa e coda.
 *
 * Layout struttura (offset):
 *   [0]   capacità  (elementi)
 *   [4]   sentinel/magic
 *   [8]   puntatore write (tail)
 *   [0xc] puntatore read  (head)
 *   [0x10…] dati
 *
 * Indirizzo originale: FUN_00102d60
 */
undefined4 RingBuffer_Init(undefined4 capacity)
{
    uRam00362b40 = capacity;
    uRam00362b48 = 0x362b50;   // write ptr → primo elemento
    uRam00362b44 = 0;
    uRam00362b4c = 0x362b50;   // read ptr  → primo elemento
    return 0x362b40;            // ritorna puntatore alla struttura
}

/*
 * RingBuffer_AdvanceWrite
 * -----------------------
 * Avanza il puntatore write, wrapping alla fine.
 * Indirizzo originale: FUN_00102d88
 */
void RingBuffer_AdvanceWrite(int *rb)
{
    int next = rb[3] + 1;
    rb[1] = rb[1] + 1;
    rb[3] = next;
    // Wrap: se puntatore supera fine buffer, torna al primo elemento
    if (next == (int)rb + rb[0] + 0x10) {
        rb[3] = (int)(rb + 4);
    }
}

/*
 * RingBuffer_AdvanceRead
 * ----------------------
 * Avanza il puntatore read, wrapping alla fine.
 * Indirizzo originale: FUN_00102dc8
 */
void RingBuffer_AdvanceRead(int *rb)
{
    int next = rb[2] + 1;
    rb[1] = rb[1] - 1;
    rb[2] = next;
    if (next == (int)rb + rb[0] + 0x10) {
        rb[2] = (int)(rb + 4);
    }
}


/* ============================================================
 *  [SYS] Thread scheduler — dispatcher TCB/EE
 * ============================================================ */

/*
 * Thread_Dispatch
 * ---------------
 * Loop del dispatcher thread: estrae eventi dalla coda e li smista
 * a WakeupThread, SuspendThread o ResumeThread.
 *
 * Tipi evento (byte nel ring buffer):
 *   0 → WakeupThread (sveglia thread)
 *   1 → RotateThreadReadyQueue (yield)
 *   2 → SuspendThread (sospende)
 *
 * Indirizzo originale: FUN_00102820
 */
void Thread_Dispatch(uint *event_queue)
{
    do {
        while (true) {
            while (true) {
                WaitSema();
                int idx  = (*event_queue & 0x1ff) * 2;
                *event_queue = (*event_queue & 0x1ff) + 1;
                byte event_type = *(byte *)((int)event_queue + idx + 8);
                if (event_type != 1) break;
                RotateThreadReadyQueue();   // yield
            }
            if (event_type >= 2) break;
            if (event_type == 0) {
                WakeupThread();
            } else {
                Debug_Printf(0x3534f8, event_type);
            }
        }
        if (event_type != 2) {
            Debug_Printf(0x3534f8, event_type);
        }
        SuspendThread();
    } while (true);
}

/*
 * Thread_Init
 * -----------
 * Crea il semaforo e il thread dispatcher.
 * Usato da sceSifInit e similari per il threading IOP↔EE.
 *
 * Indirizzo originale: FUN_001028f8
 */
int Thread_Init(void)
{
    if (iRam00317900 < 1) {
        long sema_id = CreateSema();
        uRam003626d0 = (undefined4)sema_id;
        if (-1 < sema_id) {
            long thread_id = CreateThread();
            iRam00317900 = (int)thread_id;
            if (-1 < thread_id) {
                uRam003626d8 = 0;
                uRam003626dc = 0;
                StartThread();
                GetThreadId();
                ChangeThreadPriority();
                return iRam00317900;
            }
            DeleteSema();
        }
    }
    return -1;
}


/* ============================================================
 *  [BTB] — Battle Main (btb_main.c)
 *  Gestisce il loop principale del combattimento:
 *   - Inizializzazione round
 *   - Update frame-by-frame
 *   - Gestione timer battaglia
 *   - Risultato (vittoria/sconfitta/pareggio)
 * ============================================================ */

/*
 * Battle_Init
 * -----------
 * Inizializza lo stato del round di battaglia.
 * Resetta HP, Ki, timer, flags round.
 * Indirizzo originale: area FUN_00 intorno al modulo btb_main
 * (funzione esatta da mappare con analisi avanzata Ghidra)
 */

/*
 * Battle_Update
 * -------------
 * Aggiorna ogni frame: input, fisica, animazioni, collisioni.
 * Chiamata nel main game loop ~ 60 volte/secondo.
 */

/*
 * Battle_CheckWinCondition
 * ------------------------
 * Controlla se un combattente ha vinto (HP=0 o timer=0).
 */


/* ============================================================
 *  [BTL] — Battle Command IOP (btl_cmd.c)
 *  Comandi inviati all'IOP (I/O Processor) per audio/pad.
 *  Usa la SIF (Sub-Interface) EE↔IOP via DMA channel 5.
 * ============================================================ */

/*
 * BattleCmd_SendAudioCommand
 * --------------------------
 * Invia un comando audio all'IOP (play sound, set volume, ecc.).
 * Attende che l'IOP sia pronto (polling su 0x4ecbc0).
 *
 * Indirizzo originale: area FUN_00153340
 */
undefined4 BattleCmd_SendAudioCommand(undefined4 cmd_data)
{
    int iVar1;
    undefined4 uStack_4 = cmd_data;

    // Attende IOP disponibile
    do { iVar1 = IOP_WaitReady((int *)0x4ecbc0); } while (iVar1 != 0);

    // Copia dati nel buffer SIF e invia comando 0x21
    SIF_CopyToBuffer(puGpffff88cc, (undefined8 *)&uStack_4, 4);
    iVar1 = SIF_SendCommand((undefined4 *)0x4ecbc0, 0x21, 3,
                            (long)(int)puGpffff88cc, 4, 0, 0, 0, 0);
    return (iVar1 < 0) ? 0xffffffff : 0;
}

/*
 * BattleCmd_SendSoundEffect
 * -------------------------
 * Invia un effetto sonoro con pan e volume.
 * Comando SIF 0x1f (play SFX).
 *
 * Indirizzo originale: area FUN_001533e0 (dedotto da pattern)
 */
undefined4 BattleCmd_SendSoundEffect(undefined4 sfx_id, undefined4 volume,
                                     undefined2 pan_l, undefined2 pan_r,
                                     undefined4 pitch, undefined4 loop)
{
    int iVar1;
    do { iVar1 = IOP_WaitReady((int *)0x4ecbc0); } while (iVar1 != 0);

    // Pacchetto: sfx_id, volume, pan_l/r, pitch, loop (size 0x18)
    *puGpffff88cc        = sfx_id;
    puGpffff88cc[1]      = volume;
    *(undefined2 *)(puGpffff88cc + 2)       = pan_l;
    *(undefined2 *)((int)puGpffff88cc + 10) = pan_r;
    puGpffff88cc[3]      = pitch;
    puGpffff88cc[4]      = loop;
    *(undefined2 *)(puGpffff88cc + 5)       = pan_l;
    *(undefined2 *)((int)puGpffff88cc + 0x16) = pan_r;

    iVar1 = SIF_SendCommand((undefined4 *)0x4ecbc0, 0x22, 3,
                            (long)(int)puGpffff88cc, 0x18, 0, 0, 0, 0);
    return (iVar1 < 0) ? 0xffffffff : 0;
}


/* ============================================================
 *  [ESS] — Sistema Capsule/Carte (ess_deck/list/shop/trade.c)
 *  Implementa il sistema Capsule di Budokai:
 *   - ess_deck  : gestione deck (set di abilità equip)
 *   - ess_list  : lista capsule possedute
 *   - ess_shop  : negozio (acquisto capsule con Zenie)
 *   - ess_trade : scambio capsule tra profili
 * ============================================================ */

/*
 * Deck_Init
 * ---------
 * Inizializza la struttura deck per un personaggio.
 * Azzera gli slot abilità e imposta valori default.
 */

/*
 * Deck_EquipCapsule
 * -----------------
 * Equipa una capsula in uno slot del deck.
 * Controlla compatibilità con il personaggio e slot liberi.
 */

/*
 * Deck_UnequipCapsule
 * -------------------
 * Rimuove una capsula dal deck, rimettendola nell'inventario.
 */

/*
 * Shop_BuyCapsule
 * ---------------
 * Acquista una capsula con Zenie (moneta di gioco).
 * Verifica il saldo, scala i Zenie, aggiunge all'inventario.
 */

/*
 * Shop_GetCapsulePrice
 * --------------------
 * Ritorna il prezzo di una capsula dato l'ID.
 */

/*
 * Trade_ExchangeCapsule
 * ---------------------
 * Scambia una capsula tra due inventari (2 player local).
 */


/* ============================================================
 *  [SAT] — Satan Mode / Demo (satanmode.c)
 *  Modalità speciale con Mr. Satan come personaggio giocabile.
 *  Contiene anche la logica per la demo attractor (idle demo).
 * ============================================================ */

/*
 * SatanMode_Init       → FUN_ nel range satanmode.c
 * SatanMode_MainUpdate → SATAN_DEMO_MAIN
 * SatanMode_Result     → SATAN_RESULT_MAIN
 * SatanMode_Clear      → SATAN_CLEAR_MAIN
 */


/* ============================================================
 *  [MENU] — Menu Azione / Draw (MenuActDraw.c)
 * ============================================================ */

/*
 * Menu_DrawActionList
 * -------------------
 * Disegna la lista azioni di battaglia (attacchi, difese, skill).
 * Usa sprite 2D sovrapposti al rendering 3D principale.
 * Stringa interna: "Menu_ActDraw"
 */

/*
 * Menu_DrawSprites
 * ----------------
 * Disegna gli sprite UI (barre HP, Ki, timer).
 * Stringa interna: "Menu_SprDraw"
 */


/* ============================================================
 *  [AM] — Animation Module (amMotion/amObject/amTexture.c)
 * ============================================================ */

/*
 * Motion_Update
 * -------------
 * Aggiorna l'animazione scheletale del personaggio.
 * Calcola le matrici bone per il frame corrente.
 * (amMotion.c)
 */

/*
 * Object_Init / Object_Update / Object_Destroy
 * ----------------------------------------------
 * Gestione ciclo di vita oggetti di gioco (proiettili, effetti).
 * (amObject.c)
 */

/*
 * Texture_Load / Texture_Unload
 * ------------------------------
 * Carica/scarica texture dalla VRAM del GS.
 * Usa il DMA GIF per trasferire i dati CLUT e texel.
 * (amTexture.c)
 */


/* ============================================================
 *  [AURA] — Proiettili / Aura / Beam (aurashot.c)
 * ============================================================ */

/*
 * AuraShot_Init
 * -------------
 * Inizializza un proiettile (es. Kamehameha, Galick Gun).
 * Alloca un oggetto dal pool, imposta direzione e velocità.
 * Variabili globali: UNK_00562d48 (contatore), UNK_00562d4c (max).
 */

/*
 * AuraShot_Update
 * ---------------
 * Aggiorna posizione proiettile e verifica collisioni.
 * "shot_draw" = funzione di rendering del proiettile (GIF packet).
 */

/*
 * AuraShot_Draw
 * -------------
 * Disegna il proiettile: genera il pacchetto GIF con la mesh aura.
 * Stringa interna: "shot_draw"
 */


/* ============================================================
 *  [OPT] — Opzioni di gioco (option.c)
 * ============================================================ */

/*
 * Option_Init
 * -----------
 * Carica le opzioni dalla memory card (configurazione audio, video).
 */

/*
 * Option_Save
 * -----------
 * Salva le opzioni sulla memory card.
 */

/*
 * Option_ApplyAudioSettings
 * -------------------------
 * Applica volume BGM/SFX inviando comandi all'IOP audio.
 */


/*
 * ============================================================================
 *  MAPPA INDIRIZZI → NOMI FUNZIONI (riferimento rapido)
 * ============================================================================
 *
 *  FUN_001000d0  → UInt64ToFloat_Safe
 *  FUN_00100110  → String_IsGreaterThan
 *  FUN_00100130  → GS_SetDisplayMode
 *  FUN_001002c0  → GS_GetConfigPtr
 *  FUN_001002d0  → VIF1_Reset
 *  FUN_00100338  → GS_BuildDisplayPacket
 *  FUN_001005a8  → GS_ApplyDisplayRegisters
 *  FUN_00100668  → GS_CalcTextureBufferWidth
 *  FUN_00100a20  → GIF_DmaSend
 *  FUN_00100b08  → GS_WaitVSync
 *  FUN_00100ba0  → VIF1_GIF_WaitIdle
 *  FUN_00100eb8  → GS_SetVsyncCallback
 *  FUN_00100f58  → GS_CalcFramebufferOffset_Context1
 *  FUN_001011c0  → GS_CalcFramebufferOffset_Context2
 *  FUN_00101530  → GS_PresentFrame
 *  FUN_00101e60  → Vsync_ClearFlag
 *  FUN_00101e70  → Vsync_PollInterrupt
 *  FUN_00101f00  → Vsync_WaitWithFlag
 *  FUN_001020e0  → Heap_Alloc
 *  FUN_00102190  → [stub: return 1]
 *  FUN_00102230  → Cache_WritebackRange
 *  FUN_001022d8  → Cache_WritebackRange_Safe
 *  FUN_00102370  → Cache_InvalidateRange
 *  FUN_00102418  → Cache_InvalidateRange_Safe
 *  FUN_001024c0  → Intc_DisableVsync
 *  FUN_00102528  → Intc_EnableVsync
 *  FUN_00102590  → Dmac_Disable
 *  FUN_001025f8  → Dmac_Enable
 *  FUN_00102660  → iIntc_Enable
 *  FUN_00102680  → iIntc_Disable
 *  FUN_001026a0  → iDmac_Enable
 *  FUN_001026c0  → iDmac_Disable
 *  FUN_001026f0  → [syscall 0x5a wrapper]
 *  FUN_00102748  → [bootup/RCNT3 check]
 *  FUN_00102820  → Thread_Dispatch
 *  FUN_001028f8  → Thread_Init
 *  FUN_001029d0  → Thread_EventHandler_WakeOrYield
 *  FUN_00102a68  → Thread_EventQueue_Push
 *  FUN_00102ae8  → Thread_EventHandler_SuspendOrYield
 *  FUN_00102b80  → Deci2_Open
 *  FUN_00102bc8  → Deci2_Close
 *  FUN_00102bf0  → Deci2_Send
 *  FUN_00102c20  → Deci2_Recv
 *  FUN_00102c48  → Deci2_SendAsync
 *  FUN_00102c80  → Deci2_RecvAsync
 *  FUN_00102ce8  → Deci2_Poll
 *  FUN_00102d10  → Deci2_Flush
 *  FUN_00102d38  → Deci2_GetStatus
 *  FUN_00102d60  → RingBuffer_Init
 *  FUN_00102d88  → RingBuffer_AdvanceWrite
 *  FUN_00102dc8  → RingBuffer_AdvanceRead
 *  FUN_001534d0  → TCB_Init
 *  FUN_00153340  → BattleCmd_SendAudioCommand
 *  FUN_001533e0  → BattleCmd_SendSoundEffect
 *
 * ============================================================================
 */
