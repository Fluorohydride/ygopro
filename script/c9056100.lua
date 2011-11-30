--氷結界の虎将 グルナード
function c9056100.initial_effect(c)
	--extra summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(9056100,0))
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_HAND+LOCATION_MZONE,0)
	e1:SetCode(EFFECT_EXTRA_SUMMON_COUNT)
	e1:SetCondition(c9056100.escon)
	e1:SetTarget(aux.TargetBoolFunction(Card.IsSetCard,0x2f))
	e1:SetValue(c9056100.esop)
	c:RegisterEffect(e1)
end
function c9056100.escon(e)
	return Duel.GetFlagEffect(Duel.GetTurnPlayer(),9056100)==0
end
function c9056100.esop(e,c)
	Duel.RegisterFlagEffect(c:GetControler(),9056100,RESET_PHASE+PHASE_END,0,1)
end
