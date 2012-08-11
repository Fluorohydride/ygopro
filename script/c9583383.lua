--ガガガカイザー
function c9583383.initial_effect(c)
	--atklimit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CANNOT_ATTACK)
	e1:SetCondition(c9583383.atkcon)
	c:RegisterEffect(e1)
	--unsynchroable
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_CANNOT_BE_SYNCHRO_MATERIAL)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e2:SetValue(1)
	c:RegisterEffect(e2)
	--level change
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(9583383,0))
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCountLimit(1)
	e3:SetCost(c9583383.lvcost)
	e3:SetOperation(c9583383.lvop)
	c:RegisterEffect(e3)
end
function c9583383.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x54)
end
function c9583383.atkcon(e)
	return not Duel.IsExistingMatchingCard(c9583383.cfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,e:GetHandler())
end
function c9583383.rfilter(c,tp)
	local lv=c:GetLevel()
	return lv>0 and c:IsAbleToRemoveAsCost()
		and Duel.IsExistingMatchingCard(c9583383.tfilter,tp,LOCATION_MZONE,0,1,nil,lv)
end
function c9583383.tfilter(c,clv)
	local lv=c:GetLevel()
	return lv>0 and lv~=clv and c:IsFaceup() and c:IsSetCard(0x54)
end
function c9583383.lvcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c9583383.rfilter,tp,LOCATION_GRAVE,0,1,nil,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectMatchingCard(tp,c9583383.rfilter,tp,LOCATION_GRAVE,0,1,1,nil,tp)
	local lv=g:GetFirst():GetLevel()
	Duel.SetTargetParam(lv)
	Duel.Remove(g,POS_FACEUP,REASON_COST)
end
function c9583383.lvop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c9583383.cfilter,tp,LOCATION_MZONE,0,nil)
	local lv=Duel.GetChainInfo(0,CHAININFO_TARGET_PARAM)
	local tc=g:GetFirst()
	while tc do
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_CHANGE_LEVEL)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetValue(lv)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e1)
		tc=g:GetNext()
	end
end
