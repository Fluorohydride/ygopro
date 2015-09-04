--デルタトライ
function c12079734.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(12079734,0))
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_BATTLE_DESTROYING)
	e1:SetCondition(aux.bdocon)
	e1:SetTarget(c12079734.target)
	e1:SetOperation(c12079734.operation)
	c:RegisterEffect(e1)
end
function c12079734.filter1(c,ec)
	return c:IsType(TYPE_UNION) and c:CheckEquipTarget(ec)
end
function c12079734.filter2(c)
	return c:IsFaceup() and c:IsRace(RACE_MACHINE) and c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsAbleToDeck()
end
function c12079734.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local c=e:GetHandler()
	if chkc then
		if e:GetLabel()==0 then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c12079734.filter1(chkc,c)
		else return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c12079734.filter2(chkc) end
	end
	local b1=Duel.GetLocationCount(tp,LOCATION_SZONE)>0 and c:GetUnionCount()==0
		and Duel.IsExistingTarget(c12079734.filter1,tp,LOCATION_GRAVE,0,1,nil,c)
	local b2=Duel.IsExistingTarget(c12079734.filter2,tp,LOCATION_MZONE,0,1,nil) and Duel.IsPlayerCanDraw(tp,1)
	if chk==0 then return b1 or b2 end
	local op=0
	if b1 and b2 then
		op=Duel.SelectOption(tp,aux.Stringid(12079734,1),aux.Stringid(12079734,2))
	elseif b1 then
		op=Duel.SelectOption(tp,aux.Stringid(12079734,1))
	else op=Duel.SelectOption(tp,aux.Stringid(12079734,2))+1 end
	e:SetLabel(op)
	if op==0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
		local g=Duel.SelectTarget(tp,c12079734.filter1,tp,LOCATION_GRAVE,0,1,1,nil,c)
		Duel.SetOperationInfo(0,CATEGORY_LEAVE_GRAVE,g,1,0,0)
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
		local g=Duel.SelectTarget(tp,c12079734.filter2,tp,LOCATION_MZONE,0,1,1,nil)
		Duel.SetOperationInfo(0,CATEGORY_TODECK,g,1,0,0)
		Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
	end
end
function c12079734.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if e:GetLabel()==0 then
		local c=e:GetHandler()
		if c:IsRelateToEffect(e) and c:IsFaceup() and c:GetUnionCount()==0
			and Duel.GetLocationCount(tp,LOCATION_SZONE)>0 and tc:IsRelateToEffect(e) then
			Duel.Equip(tp,tc,c,false)
			tc:SetStatus(STATUS_UNION,true)
		end
	else
		if tc:IsRelateToEffect(e) and Duel.SendtoDeck(tc,nil,2,REASON_EFFECT)~=0 then
			Duel.ShuffleDeck(tp)
			Duel.BreakEffect()
			Duel.Draw(tp,1,REASON_EFFECT)
		end
	end
end
