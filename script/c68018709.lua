--失楽の聖女
function c68018709.initial_effect(c)
	--act qp in hand
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_QP_ACT_IN_NTPHAND)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_HAND,0)
	e1:SetCountLimit(1,68018709+EFFECT_COUNT_CODE_DUEL)
	c:RegisterEffect(e1)
end
