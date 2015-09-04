--ナチュル・ガオドレイク
function c16527176.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,c16527176.synfilter,aux.NonTuner(c16527176.synfilter),1)
	c:EnableReviveLimit()
end
function c16527176.synfilter(c)
	return c:IsAttribute(ATTRIBUTE_EARTH)
end
