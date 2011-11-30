--CNo.39 希望皇ホープレイ
function c56840427.initial_effect(c)
	--xyz summon
	c:EnableReviveLimit()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_EXTRA)
	e1:SetCondition(c56840427.xyzcon)
	e1:SetOperation(c56840427.xyzop)
	e1:SetValue(SUMMON_TYPE_XYZ)
	c:RegisterEffect(e1)
	--attack up
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_ATKCHANGE)
	e2:SetDescription(aux.Stringid(56840427,0))
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c56840427.condition)
	e2:SetCost(c56840427.cost)
	e2:SetOperation(c56840427.operation)
	c:RegisterEffect(e2)
end
function c56840427.xyzfilter(c)
	return c:IsAttribute(ATTRIBUTE_LIGHT) and c:GetLevel()==4
end
function c56840427.ovfilter(c)
	return c:IsFaceup() and c:IsCode(84013237)
end
function c56840427.xyzcon(e,c)
	if c==nil then return true end
	if Duel.IsExistingMatchingCard(c56840427.ovfilter,c:GetControler(),LOCATION_MZONE,0,1,nil) then return true end
	local g=Duel.GetXyzMaterial(c)
	return g:IsExists(c56840427.xyzfilter,3,nil)
end
function c56840427.xyzop(e,tp,eg,ep,ev,re,r,rp,c)
	local g=Duel.GetXyzMaterial(c)
	local b1=g:IsExists(c56840427.xyzfilter,3,nil)
	local b2=Duel.IsExistingMatchingCard(c56840427.ovfilter,tp,LOCATION_MZONE,0,1,nil)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_XMATERIAL)
	if (b1 and b2 and Duel.SelectYesNo(tp,aux.Stringid(56840427,1))) or ((not b1) and b2) then
		local mg=Duel.SelectMatchingCard(tp,c56840427.ovfilter,tp,LOCATION_MZONE,0,1,1,nil)
		local mg2=mg:GetFirst():GetOverlayGroup()
		Duel.Overlay(c,mg2)
		Duel.Overlay(c,mg)
		mg:Merge(mg2)
		c:SetMaterial(mg)
	else
		local mg=g:FilterSelect(tp,c56840427.xyzfilter,3,3,nil)
		Duel.Overlay(c,mg)
		c:SetMaterial(mg)
	end
end
function c56840427.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetLP(tp)<=1000
end
function c56840427.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c56840427.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and c:IsFaceup() then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(500)
		e1:SetReset(RESET_EVENT+0x1ff0000+RESET_PHASE+PHASE_END)
		c:RegisterEffect(e1)
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
		local tc=Duel.SelectMatchingCard(tp,Card.IsFaceup,tp,0,LOCATION_MZONE,1,1,nil):GetFirst()
		if tc then
			local e2=Effect.CreateEffect(c)
			e2:SetType(EFFECT_TYPE_SINGLE)
			e2:SetCode(EFFECT_UPDATE_ATTACK)
			e2:SetValue(-1000)
			e2:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
			tc:RegisterEffect(e2)
		end
	end
end
