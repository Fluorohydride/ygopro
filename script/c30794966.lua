--聖刻龍－ウシルドラゴン
function c30794966.initial_effect(c)
	--spsummon from hand
	local e1=Effect.CreateEffect(c)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_HAND)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetCondition(c30794966.hspcon)
	e1:SetOperation(c30794966.hspop)
	c:RegisterEffect(e1)
	--destroy replace
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_DESTROY_REPLACE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetTarget(c30794966.desreptg)
	e2:SetOperation(c30794966.desrepop)
	c:RegisterEffect(e2)
end
function c30794966.rfilter1(c,tp)
	return c:IsRace(RACE_DRAGON) and c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsAbleToRemoveAsCost()
		and Duel.IsExistingMatchingCard(c30794966.rfilter2,tp,LOCATION_GRAVE,0,1,c)
end
function c30794966.rfilter2(c)
	return c:IsRace(RACE_DRAGON) and c:IsType(TYPE_NORMAL) and c:IsAbleToRemoveAsCost()
end
function c30794966.hspcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c30794966.rfilter1,tp,LOCATION_GRAVE,0,1,nil,tp)
end
function c30794966.hspop(e,tp,eg,ep,ev,re,r,rp,c)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g1=Duel.SelectMatchingCard(tp,c30794966.rfilter1,tp,LOCATION_GRAVE,0,1,1,nil,tp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g2=Duel.SelectMatchingCard(tp,c30794966.rfilter2,tp,LOCATION_GRAVE,0,1,1,g1:GetFirst())
	g1:Merge(g2)
	Duel.Remove(g1,POS_FACEUP,REASON_COST)
end
function c30794966.repfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x69) and not c:IsStatus(STATUS_DESTROY_CONFIRMED)
end
function c30794966.desreptg(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return Duel.CheckReleaseGroup(tp,c30794966.repfilter,1,c) end
	if Duel.SelectYesNo(tp,aux.Stringid(30794966,0)) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESREPLACE)
		local g=Duel.SelectReleaseGroup(tp,c30794966.repfilter,1,1,c)
		e:SetLabelObject(g:GetFirst())
		return true
	else return false end
end
function c30794966.desrepop(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject()
	Duel.Release(tc,REASON_EFFECT)
end
