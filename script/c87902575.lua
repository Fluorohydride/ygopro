--フューチャー・ヴィジョン
function c87902575.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c87902575.clear)
	c:RegisterEffect(e1)
	local ng=Group.CreateGroup()
	ng:KeepAlive()
	e1:SetLabelObject(ng)
	--remove
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(87902575,0))
	e2:SetCategory(CATEGORY_REMOVE)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_SUMMON_SUCCESS)
	e2:SetTarget(c87902575.rmtg)
	e2:SetOperation(c87902575.rmop)
	e2:SetLabelObject(e1)
	c:RegisterEffect(e2)
	--return
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(87902575,1))
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e3:SetCountLimit(1)
	e3:SetCondition(c87902575.retcon)
	e3:SetOperation(c87902575.retop)
	e3:SetLabelObject(e1)
	c:RegisterEffect(e3)
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e4:SetCode(EVENT_PHASE_START+PHASE_MAIN1)
	e4:SetRange(LOCATION_SZONE)
	e4:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e4:SetCountLimit(1)
	e4:SetOperation(c87902575.clearop)
	e4:SetLabelObject(e1)
	c:RegisterEffect(e4)
end
function c87902575.clear(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	e:GetLabelObject():Clear()
end
function c87902575.rmtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return eg:GetFirst():IsOnField()
		and eg:GetFirst():IsCanBeEffectTarget(e) end
	Duel.SetTargetCard(eg:GetFirst())
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,eg,1,0,0)
end
function c87902575.rmop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local tc=eg:GetFirst()
	if tc:IsRelateToEffect(e) and tc:IsLocation(LOCATION_MZONE) and Duel.Remove(tc,POS_FACEUP,REASON_EFFECT+REASON_TEMPORARY)~=0 then
		tc:CreateRelation(e:GetHandler(),RESET_EVENT+0x1fe0000)
		e:GetLabelObject():GetLabelObject():AddCard(tc)
	end
end
function c87902575.retfilter(c,ec,tp)
	return c:IsRelateToCard(ec) and c:GetPreviousControler()==tp
end
function c87902575.retcon(e,tp,eg,ep,ev,re,r,rp)
	local g=e:GetLabelObject():GetLabelObject()
	return g:FilterCount(c87902575.retfilter,nil,e:GetHandler(),Duel.GetTurnPlayer())>0
end
function c87902575.retop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local p=Duel.GetTurnPlayer()
	local lg=e:GetLabelObject():GetLabelObject()
	local g=lg:Filter(c87902575.retfilter,nil,e:GetHandler(),p)
	lg:Sub(g)
	local ft=Duel.GetLocationCount(p,LOCATION_MZONE)
	if g:GetCount()>ft then
		local sg=g
		Duel.Hint(HINT_SELECTMSG,p,aux.Stringid(87902575,2))
		g=g:Select(p,ft,ft,nil)
		sg:Sub(g)
		Duel.SendtoGrave(sg,REASON_EFFECT)
	end
	local tc=g:GetFirst()
	while tc do
		Duel.ReturnToField(tc,POS_FACEUP_ATTACK)
		tc=g:GetNext()
	end
end
function c87902575.clfilter(c,ec,tp)
	return (not c:IsRelateToCard(ec)) or c:GetPreviousControler()==tp
end
function c87902575.clearop(e,tp,eg,ep,ev,re,r,rp)
	e:GetLabelObject():GetLabelObject():Remove(c87902575.clfilter,nil,e:GetHandler(),Duel.GetTurnPlayer())
end
