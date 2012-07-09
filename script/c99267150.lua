--F·G·D
function c99267150.initial_effect(c)
	--fusion material
	c:EnableReviveLimit()
	aux.AddFusionProcFunRep(c,aux.FilterBoolFunction(Card.IsRace,RACE_DRAGON),5,true)
	--battle indestructable
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e2:SetValue(c99267150.batfilter)
	c:RegisterEffect(e2)
	--spsummon condition
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e3:SetCode(EFFECT_SPSUMMON_CONDITION)
	e3:SetValue(c99267150.splimit)
	c:RegisterEffect(e3)
end
function c99267150.splimit(e,se,sp,st)
	return bit.band(st,SUMMON_TYPE_FUSION)==SUMMON_TYPE_FUSION
end
function c99267150.fscondition(e,g,gc)
	if g==nil then return false end
	if gc then return gc:IsRace(RACE_DRAGON) and g:IsExists(Card.IsRace,4,gc,RACE_DRAGON) end
	return g:IsExists(Card.IsRace,5,nil,RACE_DRAGON)
end
function c99267150.fsoperation(e,tp,eg,ep,ev,re,r,rp,gc)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
	if gc then Duel.SetFusionMaterial(eg:FilterSelect(tp,Card.IsRace,4,4,gc,RACE_DRAGON))
	else Duel.SetFusionMaterial(eg:FilterSelect(tp,Card.IsRace,5,5,nil,RACE_DRAGON)) end
end
function c99267150.batfilter(e,c)
	return c:IsAttribute(0x2f)
end
