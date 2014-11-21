--精霊獣 ラムペンタ
function c88123329.initial_effect(c)
	--spsummon limit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SPSUMMON_COST)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e1:SetCost(c88123329.spcost)
	c:RegisterEffect(e1)
	--tohand
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_REMOVE+CATEGORY_TOGRAVE)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetTarget(c88123329.target)
	e2:SetOperation(c88123329.operation)
	c:RegisterEffect(e2)
	if not c88123329.global_check then
		c88123329.global_check=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_SPSUMMON)
		ge1:SetOperation(aux.spchk1)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_CHAINING)
		ge2:SetOperation(aux.spchk2)
		Duel.RegisterEffect(ge2,0)
		local ge3=Effect.CreateEffect(c)
		ge3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge3:SetCode(EVENT_CHAIN_NEGATED)
		ge3:SetOperation(aux.spchk3)
		Duel.RegisterEffect(ge3,0)
	end
end
function c88123329.spcost(e,c,tp)
	return Duel.GetFlagEffect(tp,88123329)==0
end
function c88123329.tgfilter(c,rac)
	return c:IsSetCard(0xb5) and c:IsRace(rac) and c:IsAbleToGrave()
end
function c88123329.rmfilter(c,tp)
	return c:IsSetCard(0xb5) and c:IsAbleToRemove()
		and Duel.IsExistingMatchingCard(c88123329.tgfilter,tp,LOCATION_DECK,0,1,nil,c:GetRace())
end
function c88123329.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c88123329.rmfilter,tp,LOCATION_EXTRA,0,1,nil,tp) end
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,nil,1,tp,LOCATION_EXTRA)
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_DECK)
end
function c88123329.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectMatchingCard(tp,c88123329.rmfilter,tp,LOCATION_EXTRA,0,1,1,nil,tp)
	if g:GetCount()>0 and Duel.Remove(g,POS_FACEUP,REASON_EFFECT)~=0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		local sg=Duel.SelectMatchingCard(tp,c88123329.tgfilter,tp,LOCATION_DECK,0,1,1,nil,g:GetFirst():GetRace())
		Duel.SendtoGrave(sg,REASON_EFFECT)
	end
end
