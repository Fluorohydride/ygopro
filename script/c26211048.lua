--甲虫装機 エクサスタッグ
function c26211048.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,aux.FilterBoolFunction(Card.IsRace,RACE_INSECT),5,2)
	c:EnableReviveLimit()
	--equip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(26211048,0))
	e1:SetCategory(CATEGORY_EQUIP)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c26211048.eqcost)
	e1:SetTarget(c26211048.eqtg)
	e1:SetOperation(c26211048.eqop)
	c:RegisterEffect(e1)
end
function c26211048.eqcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c26211048.eqtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE+LOCATION_MZONE) and chkc:IsControler(1-tp) and chkc:IsType(TYPE_MONSTER) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_SZONE)>0
		and Duel.IsExistingTarget(Card.IsType,tp,0,LOCATION_GRAVE+LOCATION_MZONE,1,nil,TYPE_MONSTER) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
	local g=Duel.SelectTarget(tp,Card.IsType,tp,0,LOCATION_GRAVE+LOCATION_MZONE,1,1,nil,TYPE_MONSTER)
	if g:GetFirst():IsLocation(LOCATION_GRAVE) then
		Duel.SetOperationInfo(0,CATEGORY_LEAVE_GRAVE,g,1,0,0)
	end
end
function c26211048.eqop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if not tc:IsRelateToEffect(e) then return end
	if Duel.GetLocationCount(tp,LOCATION_SZONE)<=0 or c:IsFacedown() or not c:IsRelateToEffect(e) then
		if tc:IsLocation(LOCATION_MZONE) then Duel.SendtoGrave(tc,REASON_EFFECT) end
		return
	end
	Duel.Equip(tp,tc,c,false)
	local e1=Effect.CreateEffect(c)
	e1:SetProperty(EFFECT_FLAG_COPY_INHERIT+EFFECT_FLAG_OWNER_RELATE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_EQUIP_LIMIT)
	e1:SetReset(RESET_EVENT+0x1fe0000)
	e1:SetValue(c26211048.eqlimit)
	tc:RegisterEffect(e1)
	if tc:IsFaceup() then
		local atk=tc:GetTextAttack()/2
		if atk<0 then atk=0 end
		local e2=Effect.CreateEffect(c)
		e2:SetType(EFFECT_TYPE_EQUIP)
		e2:SetCode(EFFECT_UPDATE_ATTACK)
		e2:SetReset(RESET_EVENT+0x1fe0000)
		e2:SetValue(atk)
		tc:RegisterEffect(e2)
		local def=tc:GetTextDefence()/2
		if def<0 then def=0 end
		local e3=Effect.CreateEffect(c)
		e3:SetType(EFFECT_TYPE_EQUIP)
		e3:SetCode(EFFECT_UPDATE_DEFENCE)
		e3:SetReset(RESET_EVENT+0x1fe0000)
		e3:SetValue(def)
		tc:RegisterEffect(e3)
	end
end
function c26211048.eqlimit(e,c)
	return e:GetOwner()==c
end
