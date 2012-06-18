--輪廻転生
function c44182827.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--remove
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetProperty(EFFECT_FLAG_SET_AVAILABLE+EFFECT_FLAG_IGNORE_RANGE)
	e2:SetCode(EFFECT_TO_GRAVE_REDIRECT)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTarget(c44182827.rmtarget)
	e2:SetValue(LOCATION_DECKSHF)
	c:RegisterEffect(e2)
end
function c44182827.rmtarget(e,c)
	return c:GetReason()==REASON_RELEASE+REASON_RITUAL+REASON_EFFECT+REASON_MATERIAL
end
