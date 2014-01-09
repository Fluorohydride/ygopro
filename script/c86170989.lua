--ファルシオンβ
function c86170989.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(86170989,0))
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_DESTROYING)
	e1:SetTarget(c86170989.target)
	e1:SetOperation(c86170989.operation)
	c:RegisterEffect(e1)
end
function c86170989.filter1(c)
	return c:IsRace(RACE_MACHINE) and c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsAttackBelow(1200) and c:IsAbleToGrave()
end
function c86170989.filter2(c,e,tp)
	return c:IsRace(RACE_MACHINE) and c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsAttackBelow(1200)
		and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c86170989.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c86170989.filter2(chkc,e,tp) end
	if chk==0 then return true end
	local op=0
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(86170989,0))
	local t1=Duel.IsExistingMatchingCard(c86170989.filter1,tp,LOCATION_DECK,0,1,nil)
	local t2=Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingTarget(c86170989.filter2,tp,LOCATION_GRAVE,0,1,nil,e,tp)
	if t1 and t2 then
		op=Duel.SelectOption(tp,aux.Stringid(86170989,1),aux.Stringid(86170989,2))+1
	elseif t1 then
		Duel.SelectOption(tp,aux.Stringid(86170989,1))
		op=1
	elseif t2 then
		Duel.SelectOption(tp,aux.Stringid(86170989,2))
		op=2
	end
	e:SetLabel(op)
	if op==2 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
		local g=Duel.SelectTarget(tp,c86170989.filter2,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
		Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
		e:SetProperty(EFFECT_FLAG_CARD_TARGET)
		e:SetCategory(CATEGORY_SPECIAL_SUMMON)
	elseif op==1 then
		Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_DECK)
		e:SetProperty(0)
		e:SetCategory(CATEGORY_TOGRAVE)
	else
		e:SetProperty(0)
		e:SetCategory(0)
	end
end
function c86170989.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==2 then
		local tc=Duel.GetFirstTarget()
		if tc and tc:IsRelateToEffect(e) then
			Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
		end
	elseif e:GetLabel()==1 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		local g=Duel.SelectMatchingCard(tp,c86170989.filter1,tp,LOCATION_DECK,0,1,1,nil)
		if g:GetCount()>0 then
			Duel.SendtoGrave(g,REASON_EFFECT)
		end
	end
end
