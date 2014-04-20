--Pair Cycroid
function c16114248.initial_effect(c)
	--fusion material
	c:EnableReviveLimit()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(c16114248.fscondition)
	e1:SetOperation(c16114248.fsoperation)
	c:RegisterEffect(e1)
	--direct attack
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_DIRECT_ATTACK)
	c:RegisterEffect(e2)
end
function c16114248.filter(c)
	return c:IsRace(RACE_MACHINE)
end
function c16114248.filter2(c,g)
	return g:IsExists(Card.IsCode,1,c,c:GetCode())
end
function c16114248.fscondition(e,g,gc)
	if g==nil then return false end
	if gc then return false end
	return g:IsExists(c16114248.filter,1,nil) and g:IsExists(c16114248.filter2,1,nil,g)
end
function c16114248.fsoperation(e,tp,eg,ep,ev,re,r,rp,gc)
	local machg=eg:Filter(c16114248.filter,nil)
	local sameg=machg:Filter(c16114248.filter2,nil,machg)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
	local g1=sameg:Select(tp,1,1,nil)
	local ca=g1:GetFirst()
	local code=ca:GetCode()
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
	local g2=sameg:FilterSelect(tp,Card.IsCode,1,1,ca,code)
	g1:Merge(g2)
	Duel.SetFusionMaterial(g1)
end
