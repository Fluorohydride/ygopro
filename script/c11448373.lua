--グレイヴ・キーパー
function c11448373.initial_effect(c)
	--todeck
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_TO_GRAVE_REDIRECT)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e1:SetTarget(c11448373.rmtarget)
	e1:SetValue(LOCATION_DECKSHF)
	c:RegisterEffect(e1)
end
function c11448373.rmtarget(e,c)
	return c:IsReason(REASON_BATTLE)
end
