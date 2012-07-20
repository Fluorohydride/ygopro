--ヒーローフラッシュ！！
function c191749.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c191749.cost)
	e1:SetTarget(c191749.target)
	e1:SetOperation(c191749.activate)
	c:RegisterEffect(e1)
end
function c191749.cfilter(c,code)
	return c:IsCode(code) and c:IsAbleToRemoveAsCost()
end
function c191749.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c191749.cfilter,tp,LOCATION_GRAVE,0,1,nil,74825788)
		and Duel.IsExistingMatchingCard(c191749.cfilter,tp,LOCATION_GRAVE,0,1,nil,213326)
		and Duel.IsExistingMatchingCard(c191749.cfilter,tp,LOCATION_GRAVE,0,1,nil,37318031)
		and Duel.IsExistingMatchingCard(c191749.cfilter,tp,LOCATION_GRAVE,0,1,nil,63703130) end
	local tc1=Duel.GetFirstMatchingCard(c191749.cfilter,tp,LOCATION_GRAVE,0,nil,74825788)
	local tc2=Duel.GetFirstMatchingCard(c191749.cfilter,tp,LOCATION_GRAVE,0,nil,213326)
	local tc3=Duel.GetFirstMatchingCard(c191749.cfilter,tp,LOCATION_GRAVE,0,nil,37318031)
	local tc4=Duel.GetFirstMatchingCard(c191749.cfilter,tp,LOCATION_GRAVE,0,nil,63703130)
	local g=Group.FromCards(tc1,tc2,tc3,tc4)
	Duel.Remove(g,POS_FACEUP,REASON_COST)
end
function c191749.filter(c,e,tp)
	return c:IsSetCard(0x3008) and c:IsType(TYPE_NORMAL) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c191749.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c191749.filter,tp,LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,tp,LOCATION_DECK)
end
function c191749.dfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_NORMAL) and c:IsSetCard(0x3008)
end
function c191749.activate(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)>0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local g=Duel.SelectMatchingCard(tp,c191749.filter,tp,LOCATION_DECK,0,1,1,nil,e,tp)
		if g:GetCount()>0 then
			Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP)
		end
	end
	local dg=Duel.GetMatchingGroup(c191749.dfilter,tp,LOCATION_MZONE,0,nil)
	local tc=dg:GetFirst()
	while tc do
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_DIRECT_ATTACK)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1)
		tc=dg:GetNext()
	end
end
