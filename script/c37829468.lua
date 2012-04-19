--RAI－JIN
function c37829468.initial_effect(c)
	--atkup
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetTarget(c37829468.atktg)
	e1:SetValue(c37829468.atkval)
	c:RegisterEffect(e1)
	--lv change
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(37829468,0))
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_REPEAT)
	e2:SetTarget(c37829468.destg)
	e2:SetOperation(c37829468.desop)
	c:RegisterEffect(e2)
	--only 1 can exists
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_CANNOT_SUMMON)
	e3:SetCondition(c37829468.excon)
	c:RegisterEffect(e3)
	local e4=e3:Clone()
	e4:SetCode(EFFECT_CANNOT_FLIP_SUMMON)
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e5:SetCode(EFFECT_SPSUMMON_CONDITION)
	e5:SetValue(c37829468.splimit)
	c:RegisterEffect(e5)
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_SINGLE)
	e6:SetCode(EFFECT_SELF_DESTROY)
	e6:SetCondition(c37829468.descon)
	c:RegisterEffect(e6)
end
function c37829468.atktg(e,c)
	return c:IsAttribute(ATTRIBUTE_LIGHT)
end
function c37829468.atkval(e,c)
	return Duel.GetMatchingGroupCount(Card.IsAttribute,e:GetHandlerPlayer(),LOCATION_GRAVE,0,nil,ATTRIBUTE_LIGHT)*100
end
function c37829468.filter(c)
	return c:IsFaceup() and c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsDestructable()
end
function c37829468.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c37829468.filter(chkc) end
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c37829468.filter,tp,LOCATION_MZONE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c37829468.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
function c37829468.exfilter(c,fid)
	return c:IsFaceup() and c:GetCode()==37829468 and (fid==nil or c:GetFieldID()<fid)
end
function c37829468.excon(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c37829468.exfilter,c:GetControler(),LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil)
end
function c37829468.splimit(e,se,sp,st,spos,tgp)
	if bit.band(spos,POS_FACEDOWN)~=0 then return true end
	return not Duel.IsExistingMatchingCard(c37829468.exfilter,tgp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil)
end
function c37829468.descon(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c37829468.exfilter,c:GetControler(),LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil,c:GetFieldID())
end
