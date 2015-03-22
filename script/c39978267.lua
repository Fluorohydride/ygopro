--サイバー・レイダー
function c39978267.initial_effect(c)
	--effect
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(39978267,0))
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_EQUIP)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetTarget(c39978267.target)
	e1:SetOperation(c39978267.operation)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	c:RegisterEffect(e2)
	local e3=e1:Clone()
	e3:SetCode(EVENT_FLIP_SUMMON_SUCCESS)
	c:RegisterEffect(e3)
end
function c39978267.desfilter(c)
	return c:IsType(TYPE_EQUIP) and c:IsDestructable()
end
function c39978267.eqfilter(c,ec)
	return c:IsType(TYPE_EQUIP) and c:CheckEquipTarget(ec)
end
function c39978267.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then
		if e:GetLabel()==1 then return chkc:IsLocation(LOCATION_SZONE) and c39978267.desfilter(chkc)
		else return chkc:IsLocation(LOCATION_SZONE) and c39978267.eqfilter(chkc,e:GetHandler()) end
	end
	if chk==0 then return true end
	local sel=0
	if Duel.IsExistingMatchingCard(c39978267.desfilter,tp,LOCATION_SZONE,LOCATION_SZONE,1,nil) then sel=sel+1 end
	if Duel.IsExistingMatchingCard(c39978267.eqfilter,tp,LOCATION_SZONE,LOCATION_SZONE,1,nil,e:GetHandler()) then sel=sel+2 end
	if sel==3 then
		Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(39978267,0))
		sel=Duel.SelectOption(tp,aux.Stringid(39978267,1),aux.Stringid(39978267,2))+1
	end
	e:SetLabel(sel)
	if sel==1 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
		local g=Duel.SelectTarget(tp,c39978267.desfilter,tp,LOCATION_SZONE,LOCATION_SZONE,1,1,nil)
		if g:GetCount()>0 then
			Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
		end
	elseif sel==2 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
		local g=Duel.SelectTarget(tp,c39978267.eqfilter,tp,LOCATION_SZONE,LOCATION_SZONE,1,1,nil,e:GetHandler())
		if g:GetCount()>0 then
			Duel.SetOperationInfo(0,CATEGORY_EQUIP,nil,1,0,0)
		end
	end
end
function c39978267.operation(e,tp,eg,ep,ev,re,r,rp)
	local sel=e:GetLabel()
	if sel==0 then return end
	local tc=Duel.GetFirstTarget()
	if sel==1 then
		if tc and tc:IsRelateToEffect(e) then
			Duel.Destroy(tc,REASON_EFFECT)
		end
	else
		local c=e:GetHandler()
		if tc and tc:IsRelateToEffect(e) and c:IsRelateToEffect(e) and c:IsFaceup() then
			Duel.Equip(tp,tc,e:GetHandler())
		end
	end
end
