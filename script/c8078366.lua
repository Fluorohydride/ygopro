--海皇の突撃兵
function c8078366.initial_effect(c)
	--ayk
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c8078366.atkcon)
	e1:SetValue(800)
	c:RegisterEffect(e1)
end
function c8078366.cfilter(c)
	return c:IsFaceup() and c:IsRace(RACE_FISH+RACE_SEASERPENT+RACE_AQUA)
end
function c8078366.atkcon(e)
	return Duel.IsExistingMatchingCard(c8078366.cfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,e:GetHandler())
end
