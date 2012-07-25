--¥¸¥§¥¹¥¿©`¡¤¥³¥ó¥Õ¥£
function c8487449.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_SPSUM_PARAM+EFFECT_FLAG_UNCOPYABLE)
	e1:SetTargetRange(POS_FACEUP_ATTACK,0)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c8487449.spcon)
	e1:SetValue(1)
	c:RegisterEffect(e1)
	--spsummon success
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetCondition(c8487449.regcon)
	e2:SetOperation(c8487449.regop)
	c:RegisterEffect(e2)
	--only 1 can exists
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_CANNOT_SUMMON)
	e3:SetCondition(c8487449.excon)
	c:RegisterEffect(e3)
	local e4=e3:Clone()
	e4:SetCode(EFFECT_CANNOT_FLIP_SUMMON)
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e5:SetCode(EFFECT_SPSUMMON_CONDITION)
	e5:SetValue(c8487449.splimit)
	c:RegisterEffect(e5)
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_SINGLE)
	e6:SetCode(EFFECT_SELF_DESTROY)
	e6:SetCondition(c8487449.descon)
	c:RegisterEffect(e6)
end
function c8487449.spcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0
		and not Duel.IsExistingMatchingCard(c8487449.exfilter,c:GetControler(),LOCATION_ONFIELD,0,1,nil)
end
function c8487449.regcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_SPECIAL+1
end
function c8487449.regop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(8487449,0))
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetProperty(EFFECT_FLAG_REPEAT)
	e1:SetCountLimit(1)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c8487449.thcon)
	e1:SetTarget(c8487449.thtg)
	e1:SetOperation(c8487449.thop)
	e1:SetReset(RESET_EVENT+0x16a0000+RESET_PHASE+PHASE_END,2)
	e1:SetLabel(Duel.GetTurnCount())
	c:RegisterEffect(e1)
end
function c8487449.thcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnCount()==e:GetLabel()+1
end
function c8487449.filter(c)
	return c:IsFaceup() and c:IsAbleToHand()
end
function c8487449.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c8487449.filter(chkc) end
	if chk==0 then return true end
	local c=e:GetHandler()
	local sp=c:GetSummonPlayer()
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local g=Duel.SelectTarget(tp,c8487449.filter,sp,0,LOCATION_MZONE,1,1,e:GetHandler())
	if g:GetCount()>0 then
		g:AddCard(e:GetHandler())
		Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,2,0,0)
	end
end
function c8487449.thop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if tc and c:IsFaceup() and c:IsRelateToEffect(e) and tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local g=Group.FromCards(c,tc)
		Duel.SendtoHand(g,nil,REASON_EFFECT)
	end
end
function c8487449.exfilter(c,fid)
	return c:IsFaceup() and c:GetCode()==8487449 and (fid==nil or c:GetFieldID()<fid)
end
function c8487449.excon(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c8487449.exfilter,c:GetControler(),LOCATION_ONFIELD,0,1,nil)
end
function c8487449.splimit(e,se,sp,st,spos,tgp)
	if bit.band(spos,POS_FACEDOWN)~=0 then return true end
	return not Duel.IsExistingMatchingCard(c8487449.exfilter,tgp,LOCATION_ONFIELD,0,1,nil)
end
function c8487449.descon(e)
	local c=e:GetHandler()
	return Duel.IsExistingMatchingCard(c8487449.exfilter,c:GetControler(),LOCATION_ONFIELD,0,1,nil,c:GetFieldID())
end
