--森の狩人イエロー·バブーン
function c65303664.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetRange(LOCATION_HAND)
	e1:SetCode(EVENT_DESTROY)
	e1:SetOperation(c65303664.adjop)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(65303664,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetRange(LOCATION_HAND)
	e2:SetCode(EVENT_BATTLE_DESTROYED)
	e2:SetLabelObject(e1)
	e2:SetCondition(c65303664.condition)
	e2:SetCost(c65303664.cost)
	e2:SetTarget(c65303664.target)
	e2:SetOperation(c65303664.operation)
	c:RegisterEffect(e2)
end
function c65303664.filter(c,tp)
	return c:IsLocation(LOCATION_MZONE) and c:IsFaceup() and c:IsControler(tp) and c:IsRace(RACE_BEAST)
end
function c65303664.adjop(e,tp,eg,ep,ev,re,r,rp)
	local pg=e:GetLabelObject()
	if pg then pg:DeleteGroup() end
	local dg=eg:Filter(c65303664.filter,nil,tp)
	e:SetLabelObject(dg)
	dg:KeepAlive()
end
function c65303664.cfilter(c,dg)
	return c:IsLocation(LOCATION_GRAVE) and c:IsType(TYPE_MONSTER) and c:IsRace(RACE_BEAST) and dg:IsContains(c)
end
function c65303664.condition(e,tp,eg,ep,ev,re,r,rp)
	local de=e:GetLabelObject()
	local dg=de:GetLabelObject()
	if not dg then return false end
	local res=eg:IsExists(c65303664.cfilter,1,nil,dg)
	dg:DeleteGroup()
	de:SetLabelObject(nil)
	return res
end
function c65303664.rfiletr(c)
	return c:IsRace(RACE_BEAST) and c:IsAbleToRemoveAsCost()
end
function c65303664.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c65303664.rfiletr,tp,LOCATION_GRAVE,0,2,nil) end
	Duel.ConfirmCards(1-tp,e:GetHandler())
	Duel.ShuffleHand(tp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectMatchingCard(tp,c65303664.rfiletr,tp,LOCATION_GRAVE,0,2,2,nil)
	Duel.Remove(g,POS_FACEUP,REASON_COST)
end
function c65303664.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),0,0,0)
end
function c65303664.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) then
		Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEUP)
	end
end
