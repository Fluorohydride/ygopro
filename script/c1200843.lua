--八汰鏡
function c1200843.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_EQUIP)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c1200843.target)
	e1:SetOperation(c1200843.operation)
	c:RegisterEffect(e1)
	--spirit may not return
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_EQUIP)
	e2:SetCode(EFFECT_SPIRIT_MAYNOT_RETURN)
	e2:SetRange(LOCATION_SZONE)
	c:RegisterEffect(e2)
	--Equip limit
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_EQUIP_LIMIT)
	e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e3:SetValue(c1200843.eqlimit)
	c:RegisterEffect(e3)
	--destroy sub
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_EQUIP)
	e4:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e4:SetCode(EFFECT_DESTROY_SUBSTITUTE)
	e4:SetValue(c1200843.desval)
	c:RegisterEffect(e4)
end
function c1200843.eqlimit(e,c)
	return c:IsType(TYPE_SPIRIT)
end
function c1200843.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_SPIRIT)
end
function c1200843.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c1200843.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c1200843.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
	Duel.SelectTarget(tp,c1200843.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
end
function c1200843.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and tc:IsRelateToEffect(e) and tc:IsFaceup() then
		Duel.Equip(tp,c,tc)
	end
end
function c1200843.desval(e,re,r,rp)
	return bit.band(r,REASON_BATTLE)~=0
end
