--E・HERO セイラーマン
function c14225239.initial_effect(c)
	--fusion material
	c:EnableReviveLimit()
	aux.AddFusionProcCode2(c,79979666,21844576,true,true)
	--spsummon condition
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	e1:SetValue(c14225239.splimit)
	c:RegisterEffect(e1)
	--direct attack
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_DIRECT_ATTACK)
	e2:SetCondition(c14225239.dacon)
	c:RegisterEffect(e2)
end
function c14225239.splimit(e,se,sp,st)
	return bit.band(st,SUMMON_TYPE_FUSION)==SUMMON_TYPE_FUSION
end
function c14225239.filter(c)
	return c:IsFacedown() and c:GetSequence()~=5
end
function c14225239.dacon(e)
	return Duel.IsExistingMatchingCard(c14225239.filter,e:GetHandlerPlayer(),LOCATION_SZONE,0,1,nil)
end
