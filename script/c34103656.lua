--忘却の都 レミューリア
function c34103656.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--Atk/def
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTarget(c34103656.adtg)
	e2:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e2:SetValue(200)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_UPDATE_DEFENCE)
	c:RegisterEffect(e3)
	--lv
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(34103656,0))
	e4:SetType(EFFECT_TYPE_IGNITION)
	e4:SetRange(LOCATION_SZONE)
	e4:SetCountLimit(1)
	e4:SetTarget(c34103656.lvtg)
	e4:SetOperation(c34103656.lvop)
	c:RegisterEffect(e4)
end
function c34103656.adtg(e,c)
	return c:IsAttribute(ATTRIBUTE_WATER)
end
function c34103656.cfilter(c)
	return c:IsFaceup() and c:IsAttribute(ATTRIBUTE_WATER) and c:GetLevel()>0
end
function c34103656.lvtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c34103656.cfilter,tp,LOCATION_MZONE,0,1,nil) end
end
function c34103656.lvfilter(c)
	return c:IsFaceup() and c:IsAttribute(ATTRIBUTE_WATER)
end
function c34103656.lvop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) then
		local lv=Duel.GetMatchingGroupCount(c34103656.lvfilter,tp,LOCATION_MZONE,0,nil)
		local g=Duel.GetFieldGroup(tp,LOCATION_MZONE,0)
		local mg,fid=g:GetMaxGroup(Card.GetFieldID)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_FIELD)
		e1:SetCode(EFFECT_UPDATE_LEVEL)
		e1:SetRange(LOCATION_SZONE)
		e1:SetTargetRange(LOCATION_MZONE,0)
		e1:SetTarget(c34103656.efftg)
		e1:SetValue(lv)
		e1:SetLabel(fid)
		e1:SetReset(RESET_EVENT+0x1ff0000+RESET_PHASE+PHASE_END)
		c:RegisterEffect(e1)
	end
end
function c34103656.efftg(e,c)
	return c:GetFieldID()<=e:GetLabel() and c:IsAttribute(ATTRIBUTE_WATER) and c:GetLevel()>0
end
