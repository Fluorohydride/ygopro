--スケイルモース
function c68087897.initial_effect(c)
	Duel.EnableGlobalFlag(GLOBALFLAG_SPSUMMON_COUNT)
	--spsummon count limit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_COUNT_LIMIT)
	e1:SetRange(LOCATION_MZONE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetTargetRange(1,1)
	e1:SetValue(1)
	c:RegisterEffect(e1)
end
