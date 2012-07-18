--レプティレス·ヴァースキ
function c16886617.initial_effect(c)
	c:EnableReviveLimit()
	--cannot special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	e1:SetValue(aux.FALSE)
	c:RegisterEffect(e1)
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_SPSUMMON_PROC)
	e2:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e2:SetRange(LOCATION_HAND)
	e2:SetCondition(c16886617.spcon)
	e2:SetOperation(c16886617.spop)
	c:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_CANNOT_FLIP_SUMMON)
	e3:SetCondition(c16886617.excon)
	c:RegisterEffect(e3)
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE)
	e4:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e4:SetRange(LOCATION_MZONE)
	e4:SetCode(EFFECT_SELF_DESTROY)
	e4:SetCondition(c16886617.sdcon)
	c:RegisterEffect(e4)
	--destroy
	local e5=Effect.CreateEffect(c)
	e5:SetDescription(aux.Stringid(16886617,0))
	e5:SetCategory(CATEGORY_DESTROY)
	e5:SetType(EFFECT_TYPE_IGNITION)
	e5:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e5:SetRange(LOCATION_MZONE)
	e5:SetCountLimit(1)
	e5:SetTarget(c16886617.destg)
	e5:SetOperation(c16886617.desop)
	c:RegisterEffect(e5)
end
function c16886617.exfilter(c,fid)
	return c:IsFaceup() and c:GetCode()==16886617 and (fid==nil or c:GetFieldID()<fid)
end
function c16886617.rfilter(c)
	return c:IsFaceup() and c:GetAttack()==0 and c:IsReleasable()
end
function c16886617.spcon(e,c)
	if c==nil then return not Duel.IsExistingMatchingCard(c16886617.exfilter,0,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil) end
	return Duel.IsExistingMatchingCard(c16886617.rfilter,0,LOCATION_MZONE,LOCATION_MZONE,2,nil)
end
function c16886617.spop(e,tp,eg,ep,ev,re,r,rp,c)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RELEASE)
	local g=Duel.SelectMatchingCard(tp,c16886617.rfilter,0,LOCATION_MZONE,LOCATION_MZONE,2,2,nil)
	Duel.Release(g,REASON_COST)
end
function c16886617.excon(e)
	return Duel.IsExistingMatchingCard(c16886617.exfilter,0,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil)
end
function c16886617.sdcon(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c16886617.exfilter,0,LOCATION_ONFIELD,LOCATION_ONFIELD,1,c,c:GetFieldID())
end
function c16886617.desfilter(c)
	return c:IsFaceup() and c:IsDestructable()
end
function c16886617.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and c16886617.desfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c16886617.desfilter,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c16886617.desfilter,tp,0,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c16886617.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
