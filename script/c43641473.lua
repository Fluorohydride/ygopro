--移り気な仕立屋
function c43641473.initial_effect(c)
	--effect
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_EQUIP)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetHintTiming(0,TIMING_EQUIP)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c43641473.target)
	e1:SetOperation(c43641473.operation)
	c:RegisterEffect(e1)
end
function c43641473.tcfilter(tc,ec)
	return tc:IsFaceup() and ec:CheckEquipTarget(tc)
end
function c43641473.ecfilter(c)
	return c:IsType(TYPE_EQUIP) and c:GetEquipTarget()~=nil and Duel.IsExistingTarget(c43641473.tcfilter,0,LOCATION_MZONE,LOCATION_MZONE,1,c:GetEquipTarget(),c)
end
function c43641473.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_SZONE) and c43641473.ecfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c43641473.ecfilter,tp,LOCATION_SZONE,LOCATION_SZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(43641473,0))
	local g=Duel.SelectTarget(tp,c43641473.ecfilter,tp,LOCATION_SZONE,LOCATION_SZONE,1,1,nil)
	local ec=g:GetFirst()
	e:SetLabelObject(ec)
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(43641473,1))
	local tc=Duel.SelectTarget(tp,c43641473.tcfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,ec:GetEquipTarget(),ec)
end
function c43641473.operation(e,tp,eg,ep,ev,re,r,rp)
	local ec=e:GetLabelObject()
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local tc=g:GetFirst()
	if tc==ec then tc=g:GetNext() end
	if ec:IsFaceup() and ec:IsRelateToEffect(e) then 
		if tc:IsFaceup() and tc:IsRelateToEffect(e) then
			Duel.Equip(tp,ec,tc)
		else 
			Duel.SendtoGrave(ec,REASON_EFFECT) 
		end
	end
end
