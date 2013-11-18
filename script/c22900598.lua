--ヴァンパイア・シフト
function c22900598.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c22900598.condition)
	e1:SetCost(c22900598.cost)
	e1:SetTarget(c22900598.target)
	e1:SetOperation(c22900598.activate)
	c:RegisterEffect(e1)
end
function c22900598.condition(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetFieldCard(tp,LOCATION_SZONE,5)~=nil then return false end
	local g=Duel.GetMatchingGroup(Card.IsFaceup,tp,LOCATION_MZONE,0,nil)
	return g:GetCount()>0 and g:FilterCount(Card.IsRace,nil,RACE_ZOMBIE)==g:GetCount()
end
function c22900598.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,22900598)==0 end
	Duel.RegisterFlagEffect(tp,22900598,RESET_PHASE+PHASE_END,EFFECT_FLAG_OATH,1)
end
function c22900598.filter(c,tp)
	return c:IsCode(62188962) and c:GetActivateEffect():IsActivatable(tp)
end
function c22900598.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c22900598.filter,tp,LOCATION_DECK,0,1,nil,tp) end
end
function c22900598.spfilter(c,e,tp)
	return c:IsSetCard(0x8e) and c:IsAttribute(ATTRIBUTE_DARK) and c:IsCanBeSpecialSummoned(e,0,tp,false,false) and not c:IsHasEffect(EFFECT_NECRO_VALLEY)
end
function c22900598.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstMatchingCard(c22900598.filter,tp,LOCATION_DECK,0,nil,tp)
	if tc then
		local fc=Duel.GetFieldCard(1-tp,LOCATION_SZONE,5)
		if fc and fc:IsFaceup() then
			Duel.MoveToField(tc,tp,tp,LOCATION_SZONE,POS_FACEUP,true)
			Duel.Destroy(fc,REASON_RULE)
		else
			Duel.MoveToField(tc,tp,tp,LOCATION_SZONE,POS_FACEUP,true)
		end
		local te=tc:GetActivateEffect()
		local tep=tc:GetControler()
		local cost=te:GetCost()
		if cost then cost(te,tep,eg,ep,ev,re,r,rp,1) end
		Duel.RaiseEvent(tc,EVENT_CHAIN_SOLVED,tc:GetActivateEffect(),0,tp,tp,Duel.GetCurrentChain())
		local sg=Duel.GetMatchingGroup(c22900598.spfilter,tp,LOCATION_GRAVE,0,nil,e,tp)
		if sg:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(22900598,0)) then
			Duel.BreakEffect()
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
			local g=sg:Select(tp,1,1,nil)
			Duel.SpecialSummon(g,0,tp,tp,false,false,POS_FACEUP_DEFENCE)
		end
	end
end
